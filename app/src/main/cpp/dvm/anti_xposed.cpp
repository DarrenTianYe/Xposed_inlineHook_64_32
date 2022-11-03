//
// Created by darren on 2022/7/21.
//

#include <jni.h>
#include <stdlib.h>
#include <pty.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <ctype.h>
#include <stdlib.h>
#include <android/log.h>
#include <elf.h>
#include <link.h>
#include <stdbool.h>
#include <dlfcn.h>



#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "DVM", __VA_ARGS__))
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "DVM", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "DVM", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "DVM", __VA_ARGS__))


#define DCHECK(...)

#define OVERRIDE override

#define ATTRIBUTE_UNUSED __attribute__((__unused__))

#define REQUIRES_SHARED(...)

#define MANAGED PACKED(4)
#define PACKED(x) __attribute__ ((__aligned__(x), __packed__))


#ifndef NDEBUG
#define ALWAYS_INLINE
#else
#define ALWAYS_INLINE  __attribute__ ((always_inline))
#endif

char* destination = NULL;

int pResult_size =11024; //256*30+40 ~~ 7720

void appendDataStr2(char * pResult, char const * values)
{

    //LOGE(" appendDataStr result: <<<<<<<<<%s",values);
    if (strlen(pResult) > pResult_size){
        return;
    }
    strcat(pResult, values);
}

void appendDataInt2(char * pResult, long ret)
{
    if (strlen(pResult) > pResult_size){
        return;
    }
    char str[15];
    sprintf(str, ">%ld", ret);
    strcat(pResult, str);
}

void debug(C_JNIEnv *env, const char *format, jobject object) {
    if (object == nullptr) {
        LOGI(format, nullptr);
        // LOGD("darren debug null=%p", object);
    } else {
        jclass objectClass = (*env)->FindClass((JNIEnv *) env, "java/lang/Object");
        jmethodID toString = (*env)->GetMethodID((JNIEnv *) env, objectClass, "toString", "()Ljava/lang/String;");
        auto string = (jstring) (*env)->CallObjectMethod((JNIEnv *) env, object, toString);
        const char *value = (*env)->GetStringUTFChars((JNIEnv *) env, string, nullptr);
        LOGI(format, value);

        if (destination != NULL && value != NULL && (strstr(value, ".dex")
                                                     || strstr(value, ".apk")
                                                     || strstr(value, ".jar")
                                                     || strstr(value, ".so"))){
            appendDataStr2(destination, value);
        }
        (*env)->ReleaseStringUTFChars((JNIEnv *) env, string, value);
        (*env)->DeleteLocalRef((JNIEnv *) env, string);
        (*env)->DeleteLocalRef((JNIEnv *) env, objectClass);
    }
}

static int size;
static int index;
static intptr_t *container;

bool clear() {
    if (container) {
        free(container);
        size = 0;
        index = 0;
        container = nullptr;
        return true;
    } else {
        return false;
    }
}

bool add(intptr_t hash) {
    if (hash == 0) {
        return clear();
    }
    for (int i = 0; i < index; ++i) {
        if (container[i] == hash) {
            return false;
        }
    }
    if (index >= size) {
        size += 4;
        container = (intptr_t *) (realloc(container, size * sizeof(intptr_t)));
    }
    container[index++] = hash;
    return true;
}

namespace art {

    namespace mirror {

        class Object {

        };

        template<class MirrorType>
        class ObjPtr {

        public:
            MirrorType *Ptr() const {
                return nullptr;
            }

        };

        template<bool kPoisonReferences, class MirrorType>
        class PtrCompression {
        public:
            // Compress reference to its bit representation.
            static uint32_t Compress(MirrorType *mirror_ptr) {
                uintptr_t as_bits = reinterpret_cast<uintptr_t>(mirror_ptr);
                return static_cast<uint32_t>(kPoisonReferences ? -as_bits : as_bits);
            }

            // Uncompress an encoded reference from its bit representation.
            static MirrorType *Decompress(uint32_t ref) {
                uintptr_t as_bits = kPoisonReferences ? -ref : ref;
                return reinterpret_cast<MirrorType *>(as_bits);
            }

            // Convert an ObjPtr to a compressed reference.
            static uint32_t Compress(ObjPtr<MirrorType> ptr) REQUIRES_SHARED(Locks::mutator_lock_) {
                return Compress(ptr.Ptr());
            }
        };


        // Value type representing a reference to a mirror::Object of type MirrorType.
        template<bool kPoisonReferences, class MirrorType>
        class MANAGED ObjectReference {
        private:
            using Compression = PtrCompression<kPoisonReferences, MirrorType>;

        public:
            MirrorType *AsMirrorPtr() const {
                return Compression::Decompress(reference_);
            }

            void Assign(MirrorType *other) {
                reference_ = Compression::Compress(other);
            }

            void Assign(ObjPtr<MirrorType> ptr) REQUIRES_SHARED(Locks::mutator_lock_);

            void Clear() {
                reference_ = 0;
                DCHECK(IsNull());
            }

            bool IsNull() const {
                return reference_ == 0;
            }

            uint32_t AsVRegValue() const {
                return reference_;
            }

            static ObjectReference<kPoisonReferences, MirrorType>
            FromMirrorPtr(MirrorType *mirror_ptr)
            REQUIRES_SHARED(Locks::mutator_lock_) {
                return ObjectReference<kPoisonReferences, MirrorType>(mirror_ptr);
            }

        protected:
            explicit ObjectReference(MirrorType *mirror_ptr) REQUIRES_SHARED(Locks::mutator_lock_)
                    : reference_(Compression::Compress(mirror_ptr)) {
            }

            // The encoded reference to a mirror::Object.
            uint32_t reference_;
        };

        // Standard compressed reference used in the runtime. Used for StackReference and GC roots.
        template<class MirrorType>
        class MANAGED CompressedReference : public mirror::ObjectReference<false, MirrorType> {
        public:
            CompressedReference<MirrorType>() REQUIRES_SHARED(Locks::mutator_lock_)
                    : mirror::ObjectReference<false, MirrorType>(nullptr) {}

            static CompressedReference<MirrorType> FromMirrorPtr(MirrorType *p)
            REQUIRES_SHARED(Locks::mutator_lock_) {
                return CompressedReference<MirrorType>(p);
            }

        private:
            explicit CompressedReference(MirrorType *p) REQUIRES_SHARED(Locks::mutator_lock_)
                    : mirror::ObjectReference<false, MirrorType>(p) {}
        };
    }

    class RootInfo {

    };

    class RootVisitor {
    public:
        virtual ~RootVisitor() {}

        // Single root version, not overridable.
        ALWAYS_INLINE void VisitRoot(mirror::Object **root, const RootInfo &info)
        REQUIRES_SHARED(Locks::mutator_lock_) {
            VisitRoots(&root, 1, info);
        }

        // Single root version, not overridable.
        ALWAYS_INLINE void VisitRootIfNonNull(mirror::Object **root, const RootInfo &info)
        REQUIRES_SHARED(Locks::mutator_lock_) {
            if (*root != nullptr) {
                VisitRoot(root, info);
            }
        }

        virtual void VisitRoots(mirror::Object ***roots, size_t count, const RootInfo &info)
        REQUIRES_SHARED(Locks::mutator_lock_) = 0;

        virtual void VisitRoots(mirror::CompressedReference<mirror::Object> **roots, size_t count,
                                const RootInfo &info)
        REQUIRES_SHARED(Locks::mutator_lock_) = 0;
    };

    // Only visits roots one at a time, doesn't handle updating roots. Used when performance isn't
    // critical.
    class SingleRootVisitor : public RootVisitor {
    private:
        void VisitRoots(mirror::Object ***roots, size_t count, const RootInfo &info) OVERRIDE
        REQUIRES_SHARED(Locks::mutator_lock_) {
            for (size_t i = 0; i < count; ++i) {
                VisitRoot(*roots[i], info);
            }
        }

        void VisitRoots(mirror::CompressedReference<mirror::Object> **roots, size_t count,
                        const RootInfo &info) OVERRIDE
        REQUIRES_SHARED(Locks::mutator_lock_) {
            for (size_t i = 0; i < count; ++i) {
                VisitRoot(roots[i]->AsMirrorPtr(), info);
            }
        }

        virtual void VisitRoot(mirror::Object *root, const RootInfo &info) = 0;
    };

    class IsMarkedVisitor {
    public:
        virtual ~IsMarkedVisitor() {}

        // Return null if an object is not marked, otherwise returns the new address of that object.
        // May return the same address as the input if the object did not move.
        virtual mirror::Object *IsMarked(mirror::Object *obj) = 0;
    };

}


#define PLT_CHECK_PLT_APP ((unsigned short) 0x1u)
#define PLT_CHECK_PLT_ALL ((unsigned short) 0x2u)
#define PLT_CHECK_NAME    ((unsigned short) 0x4u)
#define PLT_CHECK_SYM_ONE ((unsigned short) 0x8u)

typedef struct Symbol {
    unsigned short check;
    unsigned short size;
    size_t total;
    ElfW(Addr) *symbol_plt;
    ElfW(Addr) *symbol_sym;
    const char *symbol_name;
    char *names;
} Symbol;

int dl_iterate_phdr_symbol(Symbol *symbol);

void *plt_dlsym(const char *name, size_t *total);


static uint32_t gnu_hash(const uint8_t *name) {
    uint32_t h = 5381;

    while (*name) {
        h += (h << 5) + *name++;
    }
    return h;
}

static uint32_t elf_hash(const uint8_t *name) {
    uint32_t h = 0, g;

    while (*name) {
        h = (h << 4) + *name++;
        g = h & 0xf0000000;
        h ^= g;
        h ^= g >> 24;
    }

    return h;
}

static ElfW(Dyn) *find_dyn_by_tag(ElfW(Dyn) *dyn, ElfW(Sxword) tag) {
while (dyn->d_tag != DT_NULL) {
if (dyn->d_tag == tag) {
return dyn;
}
++dyn;
}
return NULL;
}

static inline bool is_global(ElfW(Sym) *sym) {
    unsigned char stb = ELF_ST_BIND(sym->st_info);
    if (stb == STB_GLOBAL || stb == STB_WEAK) {
        return sym->st_shndx != SHN_UNDEF;
    } else {
        return false;
    }
}

static ElfW(Addr) *
find_symbol(struct dl_phdr_info *info, ElfW(Dyn) *base_addr, const char *symbol) {
ElfW(Dyn) *dyn;

dyn = find_dyn_by_tag(base_addr, DT_SYMTAB);
ElfW(Sym) *dynsym = (ElfW(Sym) *) (info->dlpi_addr + dyn->d_un.d_ptr);

dyn = find_dyn_by_tag(base_addr, DT_STRTAB);
char *dynstr = (char *) (info->dlpi_addr + dyn->d_un.d_ptr);

dyn = find_dyn_by_tag(base_addr, DT_GNU_HASH);
if (dyn != NULL) {
ElfW(Word) *dt_gnu_hash = (ElfW(Word) *) (info->dlpi_addr + dyn->d_un.d_ptr);
size_t gnu_nbucket_ = dt_gnu_hash[0];
uint32_t gnu_maskwords_ = dt_gnu_hash[2];
uint32_t gnu_shift2_ = dt_gnu_hash[3];
ElfW(Addr) *gnu_bloom_filter_ = (ElfW(Addr) *) (dt_gnu_hash + 4);
uint32_t *gnu_bucket_ = (uint32_t *) (gnu_bloom_filter_ + gnu_maskwords_);
uint32_t *gnu_chain_ = gnu_bucket_ + gnu_nbucket_ - dt_gnu_hash[1];

--gnu_maskwords_;

uint32_t hash = gnu_hash((uint8_t *) symbol);
uint32_t h2 = hash >> gnu_shift2_;

uint32_t bloom_mask_bits = sizeof(ElfW(Addr)) * 8;
uint32_t word_num = (hash / bloom_mask_bits) & gnu_maskwords_;
ElfW(Addr) bloom_word = gnu_bloom_filter_[word_num];

if ((1 & (bloom_word >> (hash % bloom_mask_bits)) &
(bloom_word >> (h2 % bloom_mask_bits))) == 0) {
return NULL;
}

uint32_t n = gnu_bucket_[hash % gnu_nbucket_];

if (n == 0) {
return NULL;
}

do {
ElfW(Sym) *sym = dynsym + n;
if (((gnu_chain_[n] ^ hash) >> 1) == 0
&& is_global(sym)
&& strcmp(dynstr + sym->st_name, symbol) == 0) {
ElfW(Addr) *symbol_sym = (ElfW(Addr) *) (info->dlpi_addr + sym->st_value);
#ifdef DEBUG_PLT
LOGI("found %s(gnu+%u) in %s, %p", symbol, n, info->dlpi_name, symbol_sym);
#endif
return symbol_sym;
}
} while ((gnu_chain_[n++] & 1) == 0);

return NULL;
}

dyn = find_dyn_by_tag(base_addr, DT_HASH);
if (dyn != NULL) {
ElfW(Word) *dt_hash = (ElfW(Word) *) (info->dlpi_addr + dyn->d_un.d_ptr);
size_t nbucket_ = dt_hash[0];
uint32_t *bucket_ = dt_hash + 2;
uint32_t *chain_ = bucket_ + nbucket_;

uint32_t hash = elf_hash((uint8_t *) (symbol));
for (uint32_t n = bucket_[hash % nbucket_]; n != 0; n = chain_[n]) {
ElfW(Sym) *sym = dynsym + n;
if (is_global(sym) &&
strcmp(dynstr + sym->st_name, symbol) == 0) {
ElfW(Addr) *symbol_sym = (ElfW(Addr) *) (info->dlpi_addr + sym->st_value);
#ifdef DEBUG_PLT
LOGI("found %s(elf+%u) in %s, %p", symbol, n, info->dlpi_name, symbol_sym);
#endif
return symbol_sym;
}
}

return NULL;
}

return NULL;
}

#if defined(__LP64__)
#define Elf_Rela ElfW(Rela)
#define ELF_R_SYM ELF64_R_SYM
#else
#define Elf_Rela ElfW(Rel)
#define ELF_R_SYM ELF32_R_SYM
#endif

#ifdef DEBUG_PLT
#if defined(__x86_64__)
#define R_JUMP_SLOT R_X86_64_JUMP_SLOT
#define ELF_R_TYPE  ELF64_R_TYPE
#elif defined(__i386__)
#define R_JUMP_SLOT R_386_JMP_SLOT
#define ELF_R_TYPE  ELF32_R_TYPE
#elif defined(__arm__)
#define R_JUMP_SLOT R_ARM_JUMP_SLOT
#define ELF_R_TYPE  ELF32_R_TYPE
#elif defined(__aarch64__)
#define R_JUMP_SLOT R_AARCH64_JUMP_SLOT
#define ELF_R_TYPE  ELF64_R_TYPE
#else
#error unsupported OS
#endif
#endif

static ElfW(Addr) *find_plt(struct dl_phdr_info *info, ElfW(Dyn) *base_addr, const char *symbol) {
ElfW(Dyn) *dyn = find_dyn_by_tag(base_addr, DT_JMPREL);
if (dyn == NULL) {
return NULL;
}
Elf_Rela *dynplt = (Elf_Rela *) (info->dlpi_addr + dyn->d_un.d_ptr);

dyn = find_dyn_by_tag(base_addr, DT_SYMTAB);
ElfW(Sym) *dynsym = (ElfW(Sym) *) (info->dlpi_addr + dyn->d_un.d_ptr);

dyn = find_dyn_by_tag(base_addr, DT_STRTAB);
char *dynstr = (char *) (info->dlpi_addr + dyn->d_un.d_ptr);

dyn = find_dyn_by_tag(base_addr, DT_PLTRELSZ);
if (dyn == NULL) {
return NULL;
}
size_t count = dyn->d_un.d_val / sizeof(Elf_Rela);

for (size_t i = 0; i < count; ++i) {
Elf_Rela *plt = dynplt + i;
#ifdef DEBUG_PLT
if (ELF_R_TYPE(plt->r_info) != R_JUMP_SLOT) {
            LOGW("invalid type for plt+%zu in %s", i, info->dlpi_name);
            continue;
        }
#endif
size_t idx = ELF_R_SYM(plt->r_info);
idx = dynsym[idx].st_name;
if (strcmp(dynstr + idx, symbol) == 0) {
ElfW(Addr) *symbol_plt = (ElfW(Addr) *) (info->dlpi_addr + plt->r_offset);
#ifdef DEBUG_PLT
ElfW(Addr) *symbol_plt_value = (ElfW(Addr) *) *symbol_plt;
            LOGI("found %s(plt+%zu) in %s, %p -> %p", symbol, i, info->dlpi_name, symbol_plt,
                 symbol_plt_value);
#endif
return symbol_plt;
}
}

return NULL;
}

static inline bool isso(const char *str) {
    if (str == NULL) {
        return false;
    }
    const char *dot = strrchr(str, '.');
    return dot != NULL
           && *++dot == 's'
           && *++dot == 'o'
           && (*++dot == '\0' || *dot == '\r' || *dot == '\n');
}

static inline bool isSystem(const char *str) {
    return str != NULL
           && *str == '/'
           && *++str == 's'
           && *++str == 'y'
           && *++str == 's'
           && *++str == 't'
           && *++str == 'e'
           && *++str == 'm'
           && *++str == '/';
}

static inline bool isVendor(const char *str) {
    return str != NULL
           && *str == '/'
           && *++str == 'v'
           && *++str == 'e'
           && *++str == 'n'
           && *++str == 'd'
           && *++str == 'o'
           && *++str == 'r'
           && *++str == '/';
}

static inline bool isOem(const char *str) {
    return str != NULL
           && *str == '/'
           && *++str == 'o'
           && *++str == 'e'
           && *++str == 'm'
           && *++str == '/';
}

static inline bool isThirdParty(const char *str) {
    if (isSystem(str) || isVendor(str) || isOem(str)) {
        return false;
    } else {
        return true;
    }
}

static inline bool should_check_plt(Symbol *symbol, struct dl_phdr_info *info) {
    const char *path = info->dlpi_name;
    if (symbol->check & PLT_CHECK_PLT_ALL) {
        return true;
    } else if (symbol->check & PLT_CHECK_PLT_APP) {
        return *path != '/' || isThirdParty(path);
    } else {
        return false;
    }
}

static int callback(struct dl_phdr_info *info, __unused size_t size, void *data) {
    if (!isso(info->dlpi_name)) {
#ifdef DEBUG_PLT
        LOGW("ignore non-so: %s", info->dlpi_name);
#endif
        return 0;
    }
    Symbol *symbol = (Symbol *) data;
#if 0
    LOGI("Name: \"%s\" (%d segments)", info->dlpi_name, info->dlpi_phnum);
#endif
    ++symbol->total;
    for (ElfW(Half) phdr_idx = 0; phdr_idx < info->dlpi_phnum; ++phdr_idx) {
        ElfW(Phdr) phdr = info->dlpi_phdr[phdr_idx];
        if (phdr.p_type != PT_DYNAMIC) {
            continue;
        }
        ElfW(Dyn) *base_addr = (ElfW(Dyn) *) (info->dlpi_addr + phdr.p_vaddr);
        ElfW(Addr) *addr;
        addr = should_check_plt(symbol, info) ? find_plt(info, base_addr, symbol->symbol_name) : NULL;
        if (addr != NULL) {
            if (symbol->symbol_plt != NULL) {
                ElfW(Addr) *addr_value = (ElfW(Addr) *) *addr;
                ElfW(Addr) *symbol_plt_value = (ElfW(Addr) *) *symbol->symbol_plt;
                if (addr_value != symbol_plt_value) {
#ifdef DEBUG_PLT
                    LOGW("%s, plt %p -> %p != %p", symbol->symbol_name, addr, addr_value,
                         symbol_plt_value);
#endif
                    return 1;
                }
            }
            symbol->symbol_plt = addr;
            if (symbol->check & PLT_CHECK_NAME) {
                if (symbol->size == 0) {
                    symbol->size = 1;
                    symbol->names = (char *)calloc(1, sizeof(char *));
                } else {
                    ++symbol->size;
                    symbol->names =  (char *)realloc(symbol->names, symbol->size * sizeof(char *));
                }
#ifdef DEBUG_PLT
                LOGI("[%d]: %s", symbol->size - 1, info->dlpi_name);
#endif

                //symbol->names[symbol->size - 1] = (char*)strdup(info->dlpi_name);
            }
        }
        addr = find_symbol(info, base_addr, symbol->symbol_name);
        if (addr != NULL) {
            symbol->symbol_sym = addr;
            if (symbol->check == PLT_CHECK_SYM_ONE) {
                return PLT_CHECK_SYM_ONE;
            }
        }
        if (symbol->symbol_plt != NULL && symbol->symbol_sym != NULL) {
            ElfW(Addr) *symbol_plt_value = (ElfW(Addr) *) *symbol->symbol_plt;
            // stop if unmatch
            if (symbol_plt_value != symbol->symbol_sym) {
#ifdef DEBUG_PLT
                LOGW("%s, plt: %p -> %p != %p", symbol->symbol_name, symbol->symbol_plt,
                     symbol_plt_value, symbol->symbol_sym);
#endif
                return 1;
            }
        }
    }
    return 0;
}

void *plt_dlsym(const char *name, size_t *total) {
    Symbol symbol;
    memset(&symbol, 0, sizeof(Symbol));
    if (total == NULL) {
        symbol.check = PLT_CHECK_SYM_ONE;
    }
    symbol.symbol_name = name;
    dl_iterate_phdr_symbol(&symbol);
    if (total != NULL) {
        *total = symbol.total;
    }
    return symbol.symbol_sym;
}

int dl_iterate_phdr_symbol(Symbol *symbol) {
    int result;
#ifdef DEBUG_PLT
    LOGI("start dl_iterate_phdr: %s", symbol->symbol_name);
#endif
#if __ANDROID_API__ >= 21 || !defined(__arm__)
    result = dl_iterate_phdr(callback, symbol);
#else
    int (*dl_iterate_phdr)(int (*)(struct dl_phdr_info *, size_t, void *), void *);
    dl_iterate_phdr = (int (*)(int (*)(struct dl_phdr_info *, size_t, void *), void *))dlsym(RTLD_NEXT, "dl_iterate_phdr");
    if (dl_iterate_phdr != NULL) {
        result = dl_iterate_phdr(callback, symbol);
    } else {
        result = 0;
        void *handle = dlopen("libdl.so", RTLD_NOW);
        dl_iterate_phdr = (int (*)(int (*)(struct dl_phdr_info *, size_t, void *), void *))dlsym(handle, "dl_iterate_phdr");
        if (dl_iterate_phdr != NULL) {
            result = dl_iterate_phdr(callback, symbol);
        } else {
            LOGW("cannot dlsym dl_iterate_phdr");
        }
        dlclose(handle);
    }
#endif
#ifdef DEBUG_PLT
    LOGI("complete dl_iterate_phdr: %s", symbol->symbol_name);
#endif
    return result;
}



static jobject newLocalRef(JNIEnv *env, void *object) {
    static jobject (*NewLocalRef)(JNIEnv *, void *) = nullptr;
    if (object == nullptr) {
        return nullptr;
    }
    if (NewLocalRef == nullptr) {
        NewLocalRef = (jobject (*)(JNIEnv *, void *)) plt_dlsym("_ZN3art9JNIEnvExt11NewLocalRefEPNS_6mirror6ObjectE", nullptr);
        LOGD("NewLocalRef: %p", NewLocalRef);
    }
    if (NewLocalRef != nullptr) {
        return NewLocalRef(env, object);
    } else {
        return nullptr;
    }
}

static void deleteLocalRef(JNIEnv *env, jobject object) {
    static void (*DeleteLocalRef)(JNIEnv *, jobject) = nullptr;
    if (DeleteLocalRef == nullptr) {
        DeleteLocalRef = (void (*)(JNIEnv *, jobject)) plt_dlsym("_ZN3art9JNIEnvExt14DeleteLocalRefEP8_jobject", nullptr);
        LOGD("DeleteLocalRef: %p", DeleteLocalRef);
    }
    if (DeleteLocalRef != nullptr) {
        DeleteLocalRef(env, object);
    }
}

class ClassLoaderVisitor : public art::SingleRootVisitor {
public:
    ClassLoaderVisitor(C_JNIEnv *env, jclass classLoader) : env_(env), classLoader_(classLoader) {
    }

    void VisitRoot(art::mirror::Object *root, const art::RootInfo &info ATTRIBUTE_UNUSED) final {
        jobject object = newLocalRef((JNIEnv *) env_, (jobject) root);
        if (object != nullptr) {
            if ((*env_)->IsInstanceOf((JNIEnv *) env_, object, classLoader_)) {
                //doAntiXposed(env_, object, (intptr_t) root);
                debug(env_, "visit VisitRoot root, instance: %s", object);
            }
            deleteLocalRef((JNIEnv *) env_, object);
        }
    }

private:
    C_JNIEnv *env_;
    jclass classLoader_;
};

static void checkGlobalRef(C_JNIEnv *env, jclass clazz) {

    auto VisitRoots = (void (*)(void *, void *)) plt_dlsym("_ZN3art9JavaVMExt10VisitRootsEPNS_11E", nullptr);
#ifdef DEBUG
    LOGI("VisitRoots: %p", VisitRoots);
#endif
    if (VisitRoots == nullptr) {
        return;
    }
    JavaVM *jvm;
    (*env)->GetJavaVM((JNIEnv *) env, &jvm);
    ClassLoaderVisitor visitor(env, clazz);
    VisitRoots(jvm, &visitor);
}

class WeakClassLoaderVisitor : public art::IsMarkedVisitor {
public :
    WeakClassLoaderVisitor(C_JNIEnv *env, jclass classLoader) : env_(env), classLoader_(classLoader) {
    }

    art::mirror::Object *IsMarked(art::mirror::Object *obj) override {
        jobject object = newLocalRef((JNIEnv *) env_, (jobject) obj);
        if (object != nullptr) {
            if ((*env_)->IsInstanceOf((JNIEnv *) env_, object, classLoader_)) {
                debug(env_, "visit weak root, instance: %s", object);
                //doAntiXposed(env_, object, (intptr_t) obj);
            }
            deleteLocalRef((JNIEnv *) env_, object);
        }
        return obj;
    }

private:
    C_JNIEnv *env_;
    jclass classLoader_;
};

static void checkWeakGlobalRef(C_JNIEnv *env, jclass clazz) {
    auto SweepJniWeakGlobals = (void (*)(void *, void *)) plt_dlsym("_ZN3art9JavaVMExt19SweepJniWeakGlobalsEPNS_15IsMarkedVisitorE", nullptr);
#ifdef DEBUG
    LOGI("SweepJniWeakGlobals: %p", SweepJniWeakGlobals);
#endif
    if (SweepJniWeakGlobals == nullptr) {
        return;
    }
    JavaVM *jvm;
    (*env)->GetJavaVM((JNIEnv *) env, &jvm);
    WeakClassLoaderVisitor visitor(env, clazz);
    SweepJniWeakGlobals(jvm, &visitor);
}

void checkClassLoader(C_JNIEnv *env, int sdk) {

    if (sdk < 21) {
        return;
    }
    jclass clazz = (*env)->FindClass((JNIEnv *) env, "dalvik/system/BaseDexClassLoader");
    if ((*env)->ExceptionCheck((JNIEnv *) env)) {
#ifdef DEBUG
        (*env)->ExceptionDescribe((JNIEnv *) env);
#endif
        (*env)->ExceptionClear((JNIEnv *) env);
    }
    if (clazz == nullptr) {
        return;
    }
    debug(env, "classLoader>>: %s", clazz);
    checkGlobalRef(env, clazz);
    checkWeakGlobalRef(env, clazz);
    clear();
    (*env)->DeleteLocalRef((JNIEnv *) env, clazz);
}

enum {
    // no xposed environment.
    NO_XPOSED,
    // have xposed environment but not hooked us,
    // or unknown xposed implementation, so hooks cannot be cleared.
    FOUND_XPOSED,
    // xposed hooks cleared.
    ANTIED_XPOSED,
    // can not clear hooks.
    CAN_NOT_ANTI_XPOSED,
};

int xposed_status = NO_XPOSED;

int anti_classLoad_risk(JNIEnv *env) {

    LOGD("darren anti_classLoad_risk >>>>>>>:");
    int sdk = android_get_device_api_level();
    LOGD("dvm_entry sdk >>>>>>>: %d", sdk);
    checkClassLoader((C_JNIEnv *) env, sdk);
    LOGD("darren anti_classLoad_risk=%d", xposed_status);
    if (xposed_status == NO_XPOSED) {
        return xposed_status;
    }
    LOGD("darren anti_classLoad_risk=%d", xposed_status);
    return xposed_status;
}


#define RESULT_CELL_FORMAT      "%s#%s^"
#define RESULT_CELL_FORMAT_INT      "%s#%d^"

int dvmGetEnv1(JavaVM *g_jvm, JNIEnv **env, bool *isAttached)
{
    int ret = 0;

    do {
        //获取env
        int status = g_jvm->GetEnv((void **) env, JNI_VERSION_1_4);
        if (status < 0)
        {
            LOGE("[*] start to AttachCurrentThread with status:%d\n", status);
            g_jvm->AttachCurrentThread(env, NULL);
            if ( NULL == *env)
            {
                //返回错误码
                ret = 101;
                LOGE("[-] still cant attach thread");
                break;
            }
            *isAttached = true;
            LOGE("[*] AttachCurrentThread succ\n");
        }
        LOGE("[*] vm env :%p", *env);
    }while(0);

    return ret;
}

void appendEerror11(char * pResult, size_t cap, char const * key, int errorno)
{
    LOGE("appendEerror  key: %s, errno:%d", key, errorno);
    size_t keyBufferSize = 32;
    char keyBuffer[32] = {0};
    memset(keyBuffer, 0, keyBufferSize);
    snprintf(keyBuffer, keyBufferSize-1, "%sErr", key);
    snprintf(pResult, cap, RESULT_CELL_FORMAT_INT, keyBuffer, errorno);
}

// int dvm_entry1(JNIEnv *env) {

//     const char* key = "get_dvm_black_so";
//     int ret = 0;
//     char* pResult = NULL;
//     do
//     {
//         LOGE(" dvm_entry result: >>>>>>>>>=");
//         destination = (char*)malloc(pResult_size);
//         if (NULL == destination) {
//             LOGE("destination malloc err");
//             break;
//         }
//         memset(destination, 0, pResult_size);
//         ret= anti_classLoad_risk(env);
//         pResult = (char*)malloc(pResult_size);
//         if (NULL == pResult) {
//             LOGE("darren malloc err");
//             break;
//         }
//         memset(pResult, 0, pResult_size);
//         if ( ret == 0 ){
//             snprintf(pResult, pResult_size, RESULT_CELL_FORMAT, key, destination);
//         } else{
//             appendEerror11(pResult, 256, key, ret);
//         }
//         LOGE("dvm_entry result: <<<<<<<<<=%d, %d, %s ", ret, strlen(pResult), pResult);
//     } while (0);
//     if (destination != NULL )
//     {
//         free(destination); destination = NULL;
//     }
//     return 0;
// }


//
int dvm_entrycc(int argc, char **argv) {

    const char* key = "get_dvm_black_so";
    int ret = 0;
    char* pResult = NULL;
    do
    {
        LOGE(" dvm_entry result: >>>>>>>>>=");
        JavaVM *g_jvm = (JavaVM *)argv[0];
        jobject ctx = (jobject)argv[1];
        jobject loader = (jobject)argv[2];
        const char* key = "dvm_get_classloader";
        JNIEnv *env = NULL;
        bool isAttached = false;
        int ret = dvmGetEnv1(g_jvm, &env, &isAttached);

        destination = (char*)malloc(pResult_size);
        if (NULL == destination) {
            LOGE("destination malloc err");
            break;
        }
        memset(destination, 0, pResult_size);
        ret= anti_classLoad_risk(env);
        pResult = (char*)malloc(pResult_size);
        if (NULL == pResult) {
            LOGE("darren malloc err");
            break;
        }
        memset(pResult, 0, pResult_size);
        if ( ret == 0 ){
            snprintf(pResult, pResult_size, RESULT_CELL_FORMAT, key, destination);
        } else{
            appendEerror11(pResult, 256, key, ret);
        }
        LOGE("dvm_entry result: <<<<<<<<<=%d, %d, %s ", ret, strlen(pResult), pResult);
    } while (0);
    if (destination != NULL )
    {
        free(destination); destination = NULL;
    }
    argv[argc - 1] = pResult;
    return 0;
}
