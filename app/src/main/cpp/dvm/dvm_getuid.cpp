#include <android/log.h>
#include <jni.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

//#include "DVMModule_0511.inc"

static const char* soWhiteStrDest = "bcf.so,aaudio-aidl-cpp.so,android.frameworks.bufferhub@..so,android.frameworks.schedulerservice@..so,android.frameworks.sensorservice@..so,android.hardware.audio.common@..so,android.hardware.audio.common@.-util.so,android.hardware.audio.effect@..so,android.hardware.audio@..so,android.hardware.automotive.vehicle@..so,android.hardware.camera.common@..so,android.hardware.camera.device@..so,android.hardware.cas.native@..so,android.hardware.cas@..so,android.hardware.common-V-ndk.so,android.hardware.common-V-ndk_platform.so,android.hardware.configstore@..so,android.hardware.configstore-utils.so,android.hardware.drm@..so,android.hardware.drm-V-ndk.so,android.hardware.graphics.allocator@..so,android.hardware.graphics.allocator@.-impl.so,android.hardware.graphics.allocator-V-ndk.so,android.hardware.graphics.bufferqueue@..so,android.hardware.graphics.common@..so,android.hardware.graphics.common-V-ndk.so,android.hardware.graphics.common-V-ndk_platform.so,android.hardware.graphics.composer-V-ndk.so,android.hardware.graphics.mapper@..so,android.hardware.graphics.mapper@.-impl-..so,android.hardware.graphics.mapper@.-impl.minigbm_arcvm.so,android.hardware.graphics.mapper@.-impl.so,android.hardware.graphics.mapper@.-impl-arm.so,android.hardware.graphics.mapper@.-impl-bifrost.so,android.hardware.graphics.mapper@.-impl-mediatek.so,android.hardware.graphics.mapper@.-impl-qti-display.so,android.hardware.graphics.mapper@.-impl-ranchu.so,android.hardware.graphics.mapper@.-impl-sgr.so,android.hardware.ifaa@..so,android.hardware.input.classifier@..so,android.hardware.input.common@..so,android.hardware.light@..so,android.hardware.media.bufferpool@..so,android.hardware.media.c@..so,android.hardware.media.omx@..so,android.hardware.media@..so,android.hardware.memtrack@..so,android.hardware.memtrack@.-impl.so,android.hardware.memtrack-V-ndk.so,android.hardware.memtrack-V-ndk_platform.so,android.hardware.neuralnetworks@..so,android.hardware.power@..so,android.hardware.power-V-cpp.so,android.hardware.radio@..so,android.hardware.renderscript@..so,android.hardware.renderscript@.-impl.so,android.hardware.sensors@..so,android.hidl.allocator@..so,android.hidl.base@..so,android.hidl.memory.token@..so,android.hidl.memory@..so,android.hidl.memory@.-impl.so,android.hidl.safe_union@..so,android.hidl.token@..so,android.hidl.token@.-utils.so,android.media.audio.common.types-V-cpp.so,android.system.suspend@..so,android.system.suspend-V-ndk.so,androidfw.so,app.so,arm.graphics-V-ndk.so,arm.graphics-V-ndk_platform.so,arm.mali.platform-V-ndk.so,ashmemd_aidl_interface-cpp.so,audio_common-aidl-cpp.so,audioclient-sessioninfo-aidl-cpp.so,audioclient-types-aidl-cpp.so,audioflinger-aidl-cpp.so,audiopolicy-aidl-cpp.so,audiopolicy-types-aidl-cpp.so,av-types-aidl-cpp.so,bacd.so,base.so,c.so,capture_state_listener-aidl-cpp.so,capture_state_listener-aidl-V-cpp.so,com.blackshark.game_engine@..so,com.blackshark.oeminfo@..so,com.motorola.hardware.display.panel@..so,cutils.so,debda.so,effect-aidl-cpp.so,egl.so,eglsubAndroid.so,eglSubDriverAndroid.so,eglsub-egl_oem.so,feb.so,fm.bcm.hi.so,fm.hix.hisft.so,fm.hisi.hi.so,fm.qcom.msm.so,fm.sc.so,fm.scx.so,fm.sle.so,fm.spe.so,framework-permission-aidl-cpp.so,gallium_dri.so,gralloc.Adreno.so,gralloc.alibox.so,gralloc.amlogic.so,gralloc.apq.so,gralloc.atoll.so,gralloc.baytrail.so,gralloc.bengal.so,gralloc.bst.so,gralloc.capri.so,gralloc.ceres.so,gralloc.cros.so,gralloc.default.so,gralloc.drm.so,gralloc.emulation.so,gralloc.exynos.so,gralloc.exynosB.so,gralloc.gbm.so,gralloc.gmin.so,gralloc.goldfish.so,gralloc.hi.so,gralloc.hioem.so,gralloc.hisft.so,gralloc.imx.so,gralloc.intel.so,gralloc.intel_sw.so,gralloc.kvoem.so,gralloc.kirin.so,gralloc.kirinE.so,gralloc.kirinL.so,gralloc.kona.so,gralloc.lito.so,gralloc.minigbm_arcvm.so,gralloc.moorefield.so,gralloc.mrvl.so,gralloc.msm.so,gralloc.msma.so,gralloc.msmxa.so,gralloc.msmnile.so,gralloc.mt.img.so,gralloc.mt.so,gralloc.mtm.so,gralloc.mtt.so,gralloc.mx.so,gralloc.napoli.so,gralloc.ngr.so,gralloc.omap.so,gralloc.orlando.so,gralloc.ranchu.so,gralloc.rk.so,gralloc.rkboard.so,gralloc.rogue.so,gralloc.sc.so,gralloc.sckh.so,gralloc.sdm.so,gralloc.sle.so,gralloc.slsiap.so,gralloc.sm.so,gralloc.smdkx.so,gralloc.so,gralloc.spa.so,gralloc.spe.so,gralloc.spka.so,gralloc.suni.so,gralloc.suniwp.so,gralloc.tegra.so,gralloc.tegra.so,gralloc.titan.so,gralloc.titan.so,gralloc.trinket.so,gralloc.ud.so,gralloc.ums.so,gralloc.vbox_x.so,gralloc.x.so,gralloc_viv.imx.so,heapprofd_client_api.so,hwcomposer.mt.so,i_dri.so,igfxcmrt.so,ld-android.so,lib.so,liba.so,liba_sample.so,libavcodec.so,libavformat.so,libavutil.so,libbtrans.so,libdvrcore.so,lib_algorithm_common.so,lib_algorithm_utils.so,lib_AnimationEngine_N.so,lib_apmnative.so,lib_asb_tee.so,lib_DNSe_EP_ver.so,lib_DNSe_EP_vera.so,lib_DNSe_EP_verb.so,lib_DNSe_EP_verc.so,lib_DNSe_EP_verd.so,lib_driver_load.so,lib_hwnsd_input.so,lib_idd_log_drm_dtcpsapporo.so,lib_k_ffmpeg.so,lib_k_omx_avc.so,lib_k_omx_rv.so,lib_k_omxcore.so,lib_mm_compatibility_log_utils.so,lib_mm_critical_log.so,lib_motsensorlistener.so,lib_oeminfo_interface.so,lib_oeminfo_nv_interface.so,lib_PreGainBooster_ver.so,lib_qcom_ffmpeg.so,lib_renderControl_enc.so,lib_Samsung_Resampler.so,lib_SamsungEP_V.so,lib_SamsungVAD_v.so,lib_SoundAlive_DRendering_ver.so,lib_SoundAlive_SRC_ver.so,lib_SoundAlive_SRC_ver_x.so,lib_SoundAlive_UHD_Audio.so,lib_SoundAlive_vert.so,lib_soundaliveresampler.so,lib_SoundBooster_ver.so,lib_warning_common.so,libAEnc.so,libaacextractor.so,libaal.so,libaal_mtk.so,libaalservice.so,libaaudio.so,libaaudio_internal.so,libabacus.so,libacdbloader.so,libacdbrtac.so,libacquisition.so,libacryl.so,libActivationClient.so,libactivitymanager_aidl.so,libadapter_patch_module.so,libadapter_patch_module_framework.so,libadbconnection.so,libadbconnection_client.so,libadblock_.so,libadiertac.so,libadreno_app_profiles.so,libadreno_utils.so,libadsprpc.so,libaed.so,libaedv.so,libaes_app.so,libaesni.so,libaesni_jni.so,libagp.so,libagsecurity.so,libai.arm.so,libai.so,libai_client.so,libaicdev.so,libaim.so,libaiselector.so,libAK.so,libala_devicejni.so,libalgorithm.so,libalicomphonenumberauthsdk_core.so,libalicomphonenumberauthsdk-log-online-standard-release_alijtca_plus.so,libAliNNPython.so,libalipay_iot_auth.so,libAlipayBitmapNative.so,libalipaydecode.so,libalipayteeclient.so,libalipaywebview.so,libaliplayer-...so,libaliyun_imageutils.so,libalmkdrv.so,libalsa-intf.so,libalsps_teksun.so,libAMapSDK_MAP_v__.so,libamidi.so,libammediaext.so,libammt.so,libamnet-http.so,libamnet-mmtp.so,libamnet-mng.so,libamr_wrap.so,libamrextractor.so,libAMT_numbervoice.so,libandfix.so,libandfix_x.so,libandriodapi.so,libandroid.so,libandroid_ndk.so,libandroid_net.so,libandroid_runtime.so,libandroid_runtime_lazy.so,libandroid_ssrm.so,libandroid_uc_.so,libandroidemu.so,libandroidfw.huawei.so,libandroidfw.so,libandroidicu.so,libandroidio.so,libandroidloader.so,libandroid-phone-wallet-ichat.so,libandroid-phone-wallet-nebulauc.so,libandroid-phone-wallet-trust.so,libandroidplatform.so,libantd.so,libantd_engine.so,libantd_engine_v.so,libantd_jni.so,libant_wasm_jni.so,libantcanvas.so,libantcodec.so,libantcube.so,libantgraphic.so,libantgraphic_binding.so,libantgraphic_binding_jsi.so,libantgraphic_skia.so,libantkv.so,libantmation.so,libantmation_jsi.so,libantplayer-.-x.so,libantplayer-..so,libantplayerinf-.-x.so,libantplayerinf-..so,libantplayer-netcache.so,libantpng.so,libantsp.so,libaoc.so,libaofu.so,libap_local_search.so,libapa.so,libapa_client.so,libapa_jni.so,libapaproxy.so,libape.so,libapex.so,libapex_cmn.so,libapex_intf.so,libapex_jni.so,libapex_utils.so,libapexcmn.so,libapexinf.so,libapipipe.so,libapm.so,libapmemixer.so,libAPMUOCPLIB.so,libapp.so,libapp_...so,libappfuse.so,libapplypatch_jni.so,libaprop.so,libAPSE_...so,libAPSE_..so,libapsecurity_...so,libapssdk.so,libaptsdb.so,libaquarius.so,libAR_jni.so,libarac.so,libARC.so,libArcSoftDStickerEngine.so,libarcsoft_beautyshot.so,libarcsoft_dualcam_refocus.so,libarcsoft_preprocessor.so,libarcsoft_selfie_camera_lite.so,libArcSoftFaceAlignment.so,libardrv_dynamic.so,libArmEpic.so,libart.so,libart-apollo.so,libartbase.so,libart-compiler.so,libart-dexlayout.so,libartp_new.so,libartpalette.so,libartpalette-system.so,libartvc_jingle_peerconnection_so.so,libasfdtcp.so,libasfparser.so,libashmemd_client.so,libasound.so,libassert_tip_service.so,libAstcEnc.so,libastraExt.so,libasusscreendict.so,libasyncio.so,libatci.so,libatd_corelib.so,libathena.so,libatime.so,libAtlasListParser.so,libatlasservice.so,libatlasutilshelper.so,libatomcore.so,libatomjpeg.so,libattsdb.so,libaudcal.so,libaudio.primary.default.so,libaudio_param_parser.so,libaudioclient.so,libaudioclient_aidl_conversion.so,libaudioclientextimpl.so,libaudioclientimpl.so,libaudiocompensationfilter.so,libaudiocomponentengine.so,libaudiocustparam.so,libaudiodcrflt.so,libaudiodebugtool.so,libaudioeffect_jni.so,libaudioeffectsole.so,libaudioext.so,libaudioflinger.so,libaudiofoundation.so,libaudiohal.so,libaudiomanager.so,libaudio-merger-lib.so,libaudioom.so,libaudioparameter.so,libaudiopolicy.so,libaudioresample.so,libaudioresampler.so,libAudioScmst.so,libaudiosetting.so,libaudiospdif.so,libaudiospring.so,libaudiotoolkit.so,libaudioutilmtk.so,libaudioutils.so,libaudoiv.so,libaurora.so,libAutoEnhance.so,libautotune.huawei.so,libav_ozodecoder.so,libav_ozoencoder.so,libavb.so,libavb.so,libavb.so,libavcodec.so,libavenhancements.so,libavenhancementsFIHGSI.so,libavextractor.so,libavfmt.so,libavformat.so,libavutil.so,libaxvdec.so,libbacktrace.so,libbacktrace_libc++.so,libbaiduprotect.so,libbaiengine.so,libbaimobile_engine_sam.so,libbaimobile_pcsclite_client_sam.so,libbaimobile_pkcs_sam.so,libbaimobile_platform_sam.so,libbaimobilemw.so,libbarcodereader.so,libbarrfaceclient.so,libbase.so,libbastetmanager.so,libbattery.so,libbattlog.so,libbb_tokenservice.so,libbbcr.so,libbbk_flxinput.so,libBBKGreen.so,libbcc.so,libbccArm.so,libbcd.so,libbcinfo.so,libbcmfm_if.so,libbessound_hd_mtk.so,libbessound_mtk.so,libbessurround_mtk.so,libBifrost.so,libBifrostMqtt.so,libbigdata_utils.so,libbinary_parse.so,libbinder.so,libbinder_ndk.so,libbinderserviceproxy.so,libbinderTest.so,libbinderthreadstate.so,libbionic.so,libbitmaps.so,libBitmapTransform.so,libblackbox.so,libblas.so,libblisrc.so,libblisrc.so,libblkio.so,libblox.so,libbluedroid.so,libbluetooth.so,libbluetoothdrv.so,libbluetoothem_mtk.so,libbmmcamera.so,libbmmcamera_client.so,libbmmcamera_jni.so,libbmmcamera_utils.so,libbmmcamera_watermark.so,libbmpCapture.so,libboard_param.so,libboost.so,libboost_ext_fwk.so,libboot_optimization.so,libbootlog.so,libboringssl-compat.so,libbpf.so,libbpf_android.so,libbpf_bcc.so,libbpf_binder.so,libbpf_minimal.so,libbreakpad_native.so,libbroadcast_dmbextractorhelper.so,libbroadcast_dmbextractorparser.so,libbroadcast_DMBParserOSAL.so,libbroadcast_drmfile.so,libbroadcast_frame.so,libbroadcast_frame_tdmb.so,libbroadcast_simpletsparser.so,libbrunch.so,libbsdiff.so,libbsdisplay.so,libbsproxy.so,libbtcust.so,libbtcusttable.so,libbtem.so,libbtinfo.so,libbtm.so,libbtstd.so,libbufferd.so,libbufferhub.so,libbufferhubqueue.so,libbufferq.so,libBugly.so,libBugly_rom.so,libBugly-ext.so,libburnApi.so,libBusMagnetDetectorJNI.so,libbwc.so,libbydauto.so,libbytrace_core.z.so,libbytrace_jni.z.so,libcdcolorconvert.so,libc.so,libc_leak_detector.so,libc_malloc_debug_mtk.so,libc_sec_hisi.so,libc_sec_hisi_connectivity.so,libc_secshared.so,libc++.so,libc++_shared.so,libcam.campipe.so,libcam.exif.so,libcam.halsensor.so,libcam.iopipe.so,libcam.metadata.so,libcam.utils.sensorlistener.so,libcam.utils.so,libcam_camera_exif.so,libcam_hwutils.so,libcam_mmp.so,libcam_utils.so,libcamalgo.so,libcamdrv.so,libcamerandk.so,libcamera_check.so,libcamera_check_jni.so,libcamera_check_usb_jni.so,libcamera_client.so,libcamera_client_mtk.so,libcamera_metadata.so,libcamerabgproc-jni.so,libcameracustom.so,libcameradataclient.so,libcameraprofile.so,libcamxexternalformatutils.so,libcapstone.so,libCaptureEngine.so,libCB.so,libcc_manager.so,libcc_manager_jni.so,libcca.so,libccdn.so,libcdsprpc.so,libcdsprpc_system.so,libcec.so,libCEglimage.so,libcepri_dev.so,libcfgdevparam.so,libcgrouprc.so,libchaos.so,libchc.so,libCheshireCat.so,libchost.so,libchost_log.so,libchromium_net.so,libcklm.so,libcla.so,libclang_rt.hwasan-aarch-android.so,libclang_rt.ubsan_standalone-aarch-android.so,libclang_rt.ubsan_standalone-arm-android.so,libclang_rt.ubsan_standalone-i-android.so,libclang_rt.ubsan_standalone-x_-android.so,libclatdc.so,libcld.so,libclient.kr..so,libclient.km..so,libclient.kr..so,libclient_searcher.so,libcloudtime.so,libcneapiclient.so,libcneconn.so,libcnefeatureconfig.so,libcodec.so,libcodec_client.so,libcodec_hidl_client@..so,libcodec_vndk.so,libcodecsolution.so,libcodecsolutionhelper.so,libcolorfirmware.so,libcoloros_sau_applypatch.so,libcolorx-ctrl.so,libcolorx-loader.so,libcommon.so,libcommon_basemodule_jni_rom.so,libcommon_dcs_service.so,libcommon_time_client.so,libcommon-base.so,libcompatible.so,libcompatible_.so,libcompatible_helper.so,libcompiler_rt.so,libcon.so,libconfigdb.so,libconnectivitymanager.so,libcontrolcsc.so,libconvert_calib_params_to_bin.so,libCoreCpt.so,libcorefx.so,libcorkscrew.so,libcortexdecoder.so,libcortexhandler.so,libCPHIVmiAudio.so,libCPHMediaEngine.so,libCPHVideoEngine.so,libcrash.so,libcrashlevelmatrix.so,libcrashsdk.so,libcrashsdk.so,libcrayonomxcore.so,libcrayonplayer.so,libcrayonproxybypass.so,libcritical_log_service.so,libcrypto.so,libcrypto_fips.so,libcrypto_utils.so,libcrypto-rename.so,libcrystax.so,libcsc.so,libctrlUsb.so,libcubedebug.so,libcurl.so,libcustom_jni.so,libcustom_nvram.so,libcustom_prop.so,libcutils.so,libcvenginelite.so,libcvsd_mtk.so,libcximg.so,libcxsparse.so,libcxtc.so,libcxtc_b.so,libdencode.so,libdac_pre_processing.so,libdaclient.so,libdaclient_.so,libdashplayer.so,libdatabase_sqlcrypto.so,libdataloader.so,libdatasource.so,libdatasource_local_cache.so,libdbus.so,libdcdiming.so,libdcfdecoderjni.so,libdcm_engine.so,libdcs_depth.so,libdcs_refocus.so,libdcs_stereo_client.so,libddc.so,libdebuggerd_client.so,libdeczmt.so,libdeczmt-arm.so,libdecode.so,libdecodeafb.so,libdecodeafc.so,libdecodeb.so,libdecodebba.so,libdecodebc.so,libdecodec.so,libdecoded.so,libdecodede.so,libdecodedfc.so,libdecodeea.so,libdecodeebde.so,libdecodeebe.so,libdecodeecb.so,libdecodeef.so,libdecodef.so,libdecodefbd.so,libdecodefd.so,libdecodeff.so,libdecodeffd.so,libdecodef.so,libdecodefb.so,libDefaultFpsActor.so,libdelta_dperformance.so,libDemoCommon.so,libdetect.so,libdeviceid_..so,libdexfile.so,libdexfile_external.so,libdexfile_support.so,libdhd.so,libdiag.so,libdiag_system.so,libdiagnostic.so,libDiracAPI_SHARED.so,libdirect-coredump.so,libdisp_dejitter.so,libdisplay.so,libdisplayengine-intf-cpp.so,libdisplayengine-intf-helper.so,libdisplaymodule.so,libdisplayppalgorithm.so,libDistortion.so,libDivxDrm.so,libdk_native_client.so,libdl.so,libdl_android.so,libdlb_downmixer.so,libdlfont.so,libdlfont_crypto.so,libdlfunc.so,libdlnaplayer.so,libdmabufheap.so,libdmb_jni.so,libdmb_MPEGSL.so,libdmb_MPEGTSDemux.so,libdmb_MPEGTSDemuxApi.so,libdmbFrmIF.so,libdmbhybridclient.so,libdmd.so,libdmd_report.so,libdmiapi.so,libdng_sdk.so,libdnshostprio.so,libDoagc.so,libdocker-jni-..so,libdolbyaudioeffectnativeservice.so,libdolbyottcontrol.so,libdovi.so,libdovi_omx.so,libdowngrade.so,libdpframework.so,libdpframework_mtk.so,libdpframework_os.so,libdpframework_plat.so,libdr_check.morpho.so,libdrm.so,libdrm_amdgpu.so,libdrm_android.so,libdrm_intel.so,libdrm_nouveau.so,libdrm_radeon.so,libdrm_rockchip.so,libdrm_sgpu.so,libdrm_vivante.so,libdrmbitmap.honor.so,libdrmbitmap.huawei.so,libdrmframework.so,libdrmframework_jni.so,libdrmframeworkcommon.so,libdrminterface.so,libDRMLog.so,libdrmmtkutil.so,libdrmmtkwhitelist.so,libdrmstreamjb.so,libdrmutils.so,libdsdpcm.so,libdsi_netctrl.so,libdsm.so,libdsqnif.so,libdsutils.so,libdt_fd_forward.so,libdtcpipplayer.so,libdtcpkeyctrl.so,libdtcpsapporo.so,libdtf.so,libdtfexe.so,libdti-perfd-client_system.so,libdtx_service_manager_proxy.so,libDualShotMattingCoreLIB.so,libdump.so,libdumppcm.so,libDV.so,libdvm.so,libdymdecode.so,libdzxeyopq.smcs.so,libeinxjbnlm.so,libearsense.so,libEasyAR.so,libedid.so,libedmnativehelper.so,libEffectOmxCore.so,libeffectpolicy.so,libeffects.so,libEGL.so,libEGL_adreno.so,libEGL_adreno.so,libEGL_alibox.so,libegl_dts_wrapper.so,libEGL_emulation.so,libEGL_mali.so,libEGL_mesa.so,libEGL_MRVL.so,libEGL_mtk.so,libEGL_ngr.so,libEGL_powervr.so,libEGL_POWERVR_ROGUE.so,libEGL_POWERVR_SGX_.so,libEGL_samsung.so,libEGL_swiftshader.so,libEGL_tegra.so,libEGL_tegra_impl.so,libEGL_titan.so,libEGL_titan.so,libEGL_VIVANTE.so,libEGL_vr.so,libEGLAPSInterface.so,libeglextimpl.so,libEGLSDV.so,libeis_utils.so,libelevoc_single_rn.so,libem_audio_jni.so,libem_support_jni.so,libemoji.so,libemoji_asus_.so,libEncodeEngine.so,libenv-check.so,libepic.so,libepic.so,libEpic.so,libepmhelper.so,libesnative.so,libesnative.so,libESXEGL_adreno.so,libESXGLESv_adreno.so,libESXGLESv_CM_adreno.so,libETC.so,libevent.so,libexec.so,libexif.so,libexif_android.so,libexif_jni.so,libexifa.so,libexifmtp.so,libexp.so,libexpar.so,libexpar_jsi.so,libexpat.so,libext_blkid.so,libext_utils.so,libext_uuid.so,libextem.so,libExtendedExtractor.so,libExtendedUtils.so,libexternal.so,libextjsr.so,libextmedia_jni.so,libExtractor.AC.so,libExtractor.AIFF.so,libExtractor.APE.so,libExtractor.ASF.so,libExtractor.AVI.so,libExtractor.DSDIFF.so,libExtractor.DSF.so,libExtractor.DTS.so,libExtractor.FLAC.so,libExtractor.FLV.so,libExtractor.matroska.so,libExtractor.MP.so,libExtractor.MPEG.so,libExtractor.MPEGTS.so,libExtractor.MPEG.so,libExtractor.REALMEDIA.so,libExtractor.SCADISO.so,libExtractor.WAV.so,libexynosgraphicbuffer_core.so,libexynosgscaler.so,libExynosHWCService.so,libexynosscaler.so,libexynosutils.so,libexynosvl.so,libf.so,libfeafdf.so,libf-stock.so,libFaceRecognizeSendImage.so,libFakeCameraNdk.so,libFakeOpenGLESAudio.so,libFalconFaceDynamic.so,libFalconImg-va.so,libfalconlooks.so,libFalconSmartRecog.so,libfancymedia_parserimpl.so,libfancymedia_parserextractor.so,libfancymedia_parser.so,libfastcopy.so,libfastcvopt.so,libfastpipe.so,libfb.so,libfbc.so,libfbcnf.so,libfbdev.so,libfc_falsification.so,libfc_lkctl.so,libfc_pipe.so,libfc_rwfcc.so,libfdbus-jni.so,libfeatureio.so,libfeatureiodrv.so,libffi.so,libffmpeg.huawei.so,libffmpeg.so,libffmpeg_avcodec.so,libffmpeg_baidu.so,libffmpeg_extractor.so,libffmpeg_lenovo.so,libffmpeg_utils.so,libFFmpegExtractor.so,libffmpegextractor.so,libffmpegloader.so,libffmpegutils.so,libffscanner_plugin.so,libfihinput.so,libFIHNxp.so,libfih-opt.so,libfile_access_ndk.so,libfile_op.so,libFileMonitorService.so,libfilterUtils.so,libfimc.so,libfimgd_board.so,libfimg.so,libfingersense.so,libfingersense_wrapper.so,libfjpwcalengine.so,libfjwebviewchromium_loader.so,libflacextractor.so,libflexlayout.so,libfloatingfeature.so,libflutter.so,libflutter_...so,libflybird.so,libfm_hal.so,libfm_interface.so,libfm_jni.so,libfmjni.so,libfmq.so,libfod_debug_ext.so,libfolly_json.so,libfontconfig.so,libFonts.so,libfontutil_jni.so,libforcedark.so,libforcedarkimpl.so,libfoss_service.so,libfposervice.so,libfpspolicy.so,libframebuffer_screen.so,libframeflowload.so,libFrameRecord.so,libframeskipload.so,libframework-connectivity-jni.so,libframework-connectivity-tiramisu-jni.so,libfree-reflection.so,libfreetype.so,libfreeze.so,libfrigatebird_shared.so,libFrucSSMLib.so,libfs_mgr.so,libfsldisplay.so,libft.so,libft.so,libft_.so,libftm.so,libfvkwakci.viep.so,libfw_drmutils.so,libfwclient.so,libgaudioparam.so,libgtaudioparam.so,libguaudioparam.so,libgabi++.so,libGaeaLwp.so,libGaeaLwpOverBifrost.so,libGAL.so,libGameCollect.so,libGameDisplay.so,libGameGraphicsOpt.so,libGameXtend.so,libGameXtend.so,libgas.so,libgaya.so,libgaze.so,libgaze.so,libgbm.so,libgcanvas.so,libgcanvas_runtime.so,libgcanvas_runtime_jsi.so,libgcanvas_view.so,libgccdemangle.so,libgcu.so,libGdmaScalerPipe.so,libgdscqlnit.so,libged.so,libged_kpi.so,libged_sys.so,libgem.so,libgen_def_cert.so,libgenlock.so,libgesture_client.so,libget.so,libgetzip.so,libgfxgrab.so,libglapi.so,libglcore.so,libGLES.so,libGLES__mali.so,libGLES_android.so,libGLES_bst.so,libGLES_huawei.so,libGLES_intel.so,libGLES_mali.so,libGLES_mali_v.so,libGLES_meow.so,libGLES_MEOW.so,libGLES_mesa.so,libGLES_stereos.so,libGLES_trace.so,libGLES_vc.so,libGLESOverlay.so,libGLESv.so,libGLESv_adreno.so,libGLESv_adreno.so,libGLESv_alibox.so,libGLESv_CM.so,libGLESv_CM_adreno.so,libGLESv_CM_adreno.so,libGLESv_CM_alibox.so,libGLESv_CM_emulation.so,libGLESv_CM_mali.so,libGLESv_CM_mesa.so,libGLESv_CM_MRVL.so,libGLESv_CM_mtk.so,libGLESv_CM_ngr.so,libGLESv_CM_powervr.so,libGLESv_CM_POWERVR_ROGUE.so,libGLESv_CM_POWERVR_SGX_.so,libGLESv_CM_samsung.so,libGLESv_CM_swiftshader.so,libGLESv_CM_tegra.so,libGLESv_CM_titan.so,libGLESv_CM_titan.so,libGLESv_CM_VIVANTE.so,libGLESv_CM_vr.so,libGLESv_dbg.so,libGLESv_emulation.so,libGLESv_enc.so,libGLESv_mali.so,libGLESv_mesa.so,libGLESv_MRVL.so,libGLESv_mtk.so,libGLESv_ngr.so,libGLESv_powervr.so,libGLESv_POWERVR_ROGUE.so,libGLESv_POWERVR_SGX_.so,libGLESv_samsung.so,libGLESv_swiftshader.so,libGLESv_tegra.so,libGLESv_tegra_impl.so,libGLESv_titan.so,libGLESv_titan.so,libGLESv_VIVANTE.so,libGLESv_vr.so,libGLESvSD_adreno.so,libGLESvSC.so,libglnubia.so,libglog.so,libglog_init.so,libglrcs.so,libGLSLC.so,libglslcompiler.so,libglslcompiler_SGX_.so,libGNaviData.so,libGNaviMap.so,libGNaviMapex.so,libGNaviSearch.so,libGNaviUtils.so,libgnustl_shared.so,libGoldfishProfiler.so,libGPreqcancel_svc.so,libGPTEE.so,libgpu_aux.so,libgpuassistant_client.so,libgpucommon_vendor_client.so,libgpud.so,libgpud_sys.so,libgputex.so,libGpuWatchCore.so,libgralloc.qti.so,libgralloc_drm.so,libgralloc_ext.so,libgralloc_extra.so,libgralloc_extra_sys.so,libgralloc_metadata.so,libgralloc_screen.so,libgrallocclient.so,libgralloccore.so,libgrallocgmm.so,libgralloc-jlq.so,libgralloctypes.so,libgralloctypes_mtk.so,libgrallocutils.so,libgraphicpolicy.so,libgraphicsd_ext.so,libgraphicseditor.so,libgraphicsenv.so,libgraphite.so,libgroot.so,libgsl.so,libGSLUser.so,libgspace.so,libgui.so,libgui_debug.so,libgui_ext.so,libguiextimpl.so,libgui-plugin.so,libgUtils.so,libgyhlanzx.dder.so,libhtaudioparam.so,libhuaudioparam.so,libhal_mute.so,libhandwritten.so,libhardware.so,libhardware_legacy.so,libharfbuzz.so,libharfbuzz_ng.so,libhawkeye.so,libhci.so,libhcs_sync_client.so,libhcswarp.so,libhd.so,libhdcp.so,libhdmi.so,libhdmiclient.so,libhdrtosdrtransform.so,libheadtrackingservice.so,libheif.so,libhelixplayer.so,libherope_sa.ca.so,libhevce_sb.ca.android.so,libhiarchive.so,libhiddensound.so,libhidlallocatorutils.so,libhidlbase.so,libhidl-gen-utils.so,libhidlmemory.so,libhidltransport.so,libHidlUtil.so,libhiion.so,libhilog.so,libhilog_jni.so,libhisi_ini.so,libhisi_memset.so,libhisifm_if.so,libhisinve.so,libhitrace.so,libhitrace_jni.so,libhiview.so,libhiview_jni.so,libhme_jpeg_dec.so,libhnavenhancements.so,libhnmediacodec.so,libhoks.so,libhomodisabler.so,libhomodisablermock.so,libhonorcust.so,libHookLoader.so,libhostapd_client.so,libhostlibui.so,libhotx.so,libhoudini.so,libhp_x.so,libhp_x_.so,libHpr_LocFace_mvfd_v..so,libHpr_LocLandmark_sdm_v..so,libhpsecret.so,libhtc_framework.so,libhtcflag_native.so,libhtcflag-jni.so,libhtcutils.so,libhttppdtimedtext.so,libhuawei_secure.so,libhuawei-cpp.so,libhuaweicust.so,libhvd.so,libhw_hal.so,libhwaccessapi.so,libhwaps.so,libhwapsimpl_jni.so,libhwapsutils.so,libhwastc.so,libhwbinder.so,libhwcutils.so,libhwcutilsmodule.so,libhwdeviceinfo.so,libhwetrace_jni.so,libhwextdevice.so,libHwExtendedExtractor.so,libhwgl.so,libhwinfo.so,libhwjpeg.so,libhwjpegdec.so,libhwlog.so,libhwlog_jni.so,libhwm.so,libhwmediamonitor_client.so,libhwnetutils.so,libhwnv.so,libhwobs.so,libhwplatform.so,libhwrme_client.so,libhwrme_scene.so,libhwrmeRtgSched.so,libhwrpc.so,libhwsffmpeg.so,libhwtheme_jni.so,libhwui.so,libhwuibp.so,libhwuibridge.so,libhwuiext.so,libhwuiextimpl.so,libhwuiinsptrdl.so,libhyart.so,libhynb.so,libhyperintf.so,libhyzm.so,libIcolorconvert.so,libIAL.so,libiatlasservice.so,libiAwareFrameSched.so,libiawareperf_client.so,libiawareperf_jni.so,libiawareqossched.so,libiAwareRtgSched.so,libiAwareSdk_jni.so,libiAwareSdkAdapter.so,libiAwareSdkCore.so,libiceb.so,libICJniUtils.so,libicu.so,libicu_common.so,libicu_jni.so,libicuin.so,libicuin_android.so,libicuuc.so,libicuuc_android.so,libid_android.so,libidcardtextcut_v...so,libidd.so,libidd-plat.so,libidl.so,libifaa.so,libifaa_daemon.so,libifaa_jni.so,libIfaa_jni.so,libifaaprocessor.so,libifaaserver.so,libifaateeclientjni.so,libigdusc.so,libigfxmanager.so,libiGraphicsCore.honor.so,libiGraphicsCore.huawei.so,libiGraphicsCore.so,libijkcvengine.so,libijkdevicesengine.so,libijkeditorx.so,libijkeditor.so,libijkengine.so,libijkengine-gifx.so,libijkengine-gif.so,libijkengine-mp.so,libijkffmpegx.so,libijkffmpeg.so,libijkhevcd.so,libijkmmenginex.so,libijkmmengine.so,libijkplayer.so,libijkrecorderx.so,libijkrecorder.so,libijksdl.so,libiklm.so,libImage_jni.so,libimage_refiner.morpho.so,libimage_type_recognition.so,libimageconverter.so,libimageextractor.so,libimagefilter.so,libimagehelper.so,libimageinfo.so,libimageio.so,libimageio_plat_drv.so,libimageloader.so,libImageQualityControl.so,libimageretriever.huawei.so,libImageUtils.so,libimg_utils.so,libIMGegl.so,libIMGegl_SGX_.so,libimmlistservice.so,libImmVibeJ.so,libimonitor.so,libimonitor_jni.so,lib-imsdpl.so,libimsg_log.so,lib-imsqimf.so,lib-imsrcscm.so,lib-imsrcscmclient.so,lib-ims-rcscmjni.so,lib-imsrcscmservice.so,lib-imsxml.so,libina.preload.samsung.so,libina.so,libincfs.so,libiniparser.so,libinitHelper.so,libinitlinker.so,libinput.so,libinputextensionsimpl.so,libinputflinger.so,libinspect.so,libintelmetadatabuffer.so,libinterfaceApi.so,libion.so,libion_exynos.so,libion_google.so,libion_mtk.so,libion_mtk_sys.so,libion_ulit.so,libion_ulit_sys.so,libion-nexell.so,libiopd-client.so,libiot.so,libiotauth_security.so,libIoTSdkClientJni.so,libiot-security-kernel.so,libipc_core.z.so,libipo.so,libipp_custom.so,libIQ_Match_Lib.so,libirisConfigure.so,libiss_svc.so,libisv_profile.so,libIsysmgr.so,libitouchmanager.so,libiva_android_hal.so,libiwasm.so,libjack.so,libjackservice.so,libjackshm.so,libjanklog.z.so,libjansson.so,libjavacore.so,libjavacrypto.so,libjdwp.so,libJgJZtEE.so,libjhead.so,libjhead_jni.so,libjiagu.so,libjiagu_.so,libjingle_peerconnection_so.so,libjit.so,libjklog.so,libjni_face.so,libjni_pq_camera.so,libjni_stface_api.so,libJni_wgsgcj.so,libjni-camportal.so,libjnigraphics.so,libjpeg.so,libjpeg_android.so,libjpeg_linaro_.so,libjpeg_private.so,libjpega.so,libjpeg-alpha.so,libjpeg-alpha-oal.so,libjpeghwdec.so,libJpegOal.so,libjpeg-turbo.so,libJpgDecPipe.so,libJpgEncPipe.so,libjsc.so,libjsengine-api.so,libjsengine-loadso.so,libjsengine-platform.so,libjsi.so,libjsoncpp.so,libjunk_log_collector_service.so,libjutil.so,libjvmtiagent_arm.so,libjvmtiagent_arm.so,libKcGfxLog.so,libkCmJCAGt.so,libkeyctrl.so,libkeymaster.so,libkeymaster_messages.so,libkeystore_binder.so,libkeystore-engine.so,libkeyutils.so,libkikin.so,libkll.so,libkmgw_.so,libknox_encryption.so,libknox_remotedesktopclient.knox.samsung.so,libknox_remotedesktopclient.so,libknoxvpnfdsender.so,libkoom-java.so,libkordrminterface.so,libkordrmthumbnail.so,libktv_svc.so,libkwai-linker.so,libkwb.so,libkxqpplatform.so,libkxqpzmvmpinf.so,libkye.so,liblsgpmcixsz.so,liblad.so,libladder.so,liblaiwang-searcher.so,liblatteremotegraphics.so,liblatuner.so,liblbeclient.so,liblbeclient.so,libleakdetect.so,liblg_mwaudiozoom.so,liblg_parser_ac.so,liblg_parser_asf.so,liblg_parser_avi.so,liblg_parser_dsd_utils.so,liblg_parser_dsf.so,liblg_parser_dts.so,liblg_parser_flv.so,liblg_parser_mkv.so,liblg_parser_ogm.so,liblg_parser_qcp.so,liblgalmond.so,liblgaudioutils.so,liblgblurenginecl.so,libLGCodecOSAL.so,libLGCodecParserUtils.so,liblgDivxDrm.so,liblgdmbomxcore.so,liblgdrm.so,liblgdrm_client.lge.so,liblgdrm_client.so,liblgdrm_jni.lge.so,liblgdrm_jni.so,liblgdtv_adapt.so,liblgedsoundengineEx.so,liblge_audioeffect.so,liblge_divxdrm.so,liblge_soundnormalizerV.so,liblgemoji_jni.so,liblgemojiapi_jni.so,libLgeProductFeatures.so,libLgeProductProperties.so,liblgetars.so,liblgftmitem.so,liblgmabl.so,liblgomx_clock.so,liblgomx_render_audio_android.so,liblgomx_render_video_android_ics.so,liblgomxcore.so,liblgomxdecoder_bsac.so,liblgomxdecoder_musicam.so,libLGParserOSAL.so,liblgresampler.so,liblgRmsVolComp.so,liblgruntime.so,liblgsecclk.lge.so,liblgsecclk.so,libLGTimedText_ccparser.so,libLGTimedText_xsub.so,liblgtomadrm.so,liblgtomadrm_sf.so,liblgtomadrminfo.so,libLiftParser.so,liblinearallocpatch.so,liblistenjni.qti.so,liblistenjni.so,liblistensoundmodel.qti.so,liblistensoundmodel.so,liblive.so,liblivenessdetection_v...so,libLLVM.so,libLLVM.so,libLLVM_android.so,libllvm-glnext.so,libllvm-qcom.so,libllvm-qgl.so,liblmyigtpu.brcx.so,libloader.so,liblog.so,liblog_shim.so,liblogfile.so,liblogging.so,libloggingdevice.so,liblogProducer.so,libloommapper.so,liblow.so,liblp.so,liblpn.so,liblptcp.so,liblsm.so,liblsmclient.so,liblspd.so,liblybox_namepipe.so,liblz.so,liblzma.so,libmu.so,libmu_sys.so,libm.so,libm_android.so,libmadecode.so,libmaet.so,libmagiqueson.so,libmaipython.so,libmaipython-manager.so,libmaipython-manager-v.so,libmaipython-v.so,libMali.so,libMaliCounterCapture.so,libmanos_vxv.so,libmapbox-gl.so,libmarioExt.so,libMarioYuv.so,libmars-featureclient.so,libmarsnative.so,libmars-service.so,libmarsxlog.so,libMarvellWireless.so,libmatv_cust.so,libmc.so,libmcd.so,libMcClient.so,libmcf-foundation.so,libmcf-local.so,libmcf-qc-fd-offline.so,libmcld.so,libMcRegistry.so,libmctraster.so,libmd.so,libmdf.so,libmdmdetect.so,libmedia_jni_core.so,libmedia.so,libmedia_amlogic_support.so,libmedia_codeclist.so,libmedia_ex.so,libmedia_hdcp.so,libmedia_helper.so,libmedia_jni.honor.so,libmedia_jni.huawei.so,libmedia_jni.so,libmedia_jni_utils.so,libmedia_myos.so,libmedia_native.so,libmedia_omx.so,libmedia_omx_client.so,libMediaAudioJNI.so,libmediacapture.so,libmediacapture_jni.so,libmediacodec_vivo.so,libmediacodeclogger.so,libmediacomm@.-client.so,libmediacoreg_vivo_metadata_android.so,libmediacoreg_vivo_public.so,libmediacore.so,libmediacore_vivo.so,libmediadrm.so,libmediadrmmetrics_consumer.so,libmediadrmmetrics_full.so,libmediadrmmetrics_lite.so,libmediaextractor.so,libmediafancy_utils.so,libmediaflowx.so,libmediaflow.so,libmediaflow-base.so,libmediaflow-inf.so,libmediaflow-player.so,libmediaformatshaper.so,libmediaimpl.so,libmediameta_vivo.so,libmediametrics.so,libmediamonitor_jni.so,libmediandk.so,libmediandk_utils.so,libmediaplayerservice.so,libmediaplayerservice_myos.so,libmediarecorder.huawei.so,libmediastub.so,libmediautils.so,libmeizucamera.so,libMemAdapter.so,libmemalloc.so,libmeminfo.so,libmemleak_tracker.so,libmemoryutils.so,libmemtrack.so,libmemtrack_GL.so,libmemtrack_real.so,libmemunreachable.so,libMEOW_data.so,libMEOW_gift.so,libMEOW_qt.so,libmerged-bundles-res-.so,libmerged-slink-bundles-res.so,libmesalink-jni.so,libmetasec_ml.so,libmetricslogger.so,libmfplayerx.so,libmfpusherx.so,libmhaldrv.so,libmhalImageCodec.so,libmhalmdp.so,libmiddlevare_xmlparser.so,libmidi.so,libmidiextractor.so,libmigl.so,libmigui.so,libmillet_comm.so,libmimediacodec.so,libmin_opencv_sh.so,libminiencoder.so,libminigbm_gralloc_arcvm.so,libminikin.so,libminivm.so,libMirrorAudioService.so,libmiscta.so,libmissile.so,libmiui_runtime.so,libmiuiclassproxy.so,libmiuidiffpatcher.so,libmiuiimageutilities.so,libmiuinative.so,libmiuiprocessmanager.so,libmix_imagedecoder.so,libmixvbp.so,libmka.so,libmkvextractor.so,libMLDAP.so,libmlipay.so,libmm-abl.so,libmm-abl-oem.so,libmmal.so,libmmal_util.so,libmmal_vc.so,libMMANDKSignature.bafbc.so,libMMANDKSignature.ca.so,libMMANDKSignature.cb.so,libMMANDKSignature.deac.so,libMMANDKSignature.ebea.so,libMMANDKSignature.bce.so,libMMANDKSignature.cbdce.so,libMMANDKSignature.cdf.so,libMMANDKSignature.fbcc.so,libmmcamera_dbg.so,libmmcamera_imglib.so,libmm-color-convertor.so,libmm-disp-apis.so,libmm-dspp-utils.so,libMMFW_scone_stub.so,libmmi.so,libmmkv.so,libmmkv_.so,libMMListParser.so,libmmlistparser.so,libmmocr.so,libmmosal.so,libmmparser.so,libmmparser_lite.so,libmmparserextractor.so,libmmprofile.so,libmm-qdcm-diag.so,libmmsceneservice.so,libmn.so,libmnet.so,libmnet-http.so,libmnet-ssl.so,libmnet-tcp.so,libMNN.so,libmodalityservice_jni.so,libmodbusalalib.so,libmokee-phonelocation.so,libmokee-phoneloc-jni.so,libmokee-security.so,libmonkeysaudio.so,libmonochrome.so,libmorpho_hyperlapse_interface.so,libmosquitto.so,libmotornoise.so,libmpdec_sa.ca.so,libmpenc_sa.ca.so,libmpenc_xa.ca.so,libmpextractor.so,libmplame.so,libmpmuxer.so,libmpaas_crypto.so,libmpbase.so,libmpegextractor.so,libmpp.so,libmpvr.so,libmrdump.so,libmrdumpv.so,libmrtc_mindalgo.so,libmrvl_security.so,libmsbc_mtk.so,libmsc.so,libmt.so,libmtb.so,libmtee.so,libmtkdopt.so,libmtk_drvb.so,libmtk_mali_user.so,libmtk_mmutils.so,libmtk_symbols.so,libmtkaudio_utils.so,libmtkavenhancements.so,libmtkbtextadpadp.so,libmtkcam_fwkutils.so,libmtkcamera_client.so,libmtkjpeg.so,libmtklimiter.so,libmtkperf_client.so,libmtkshifter.so,libmtksqlite_android.so,libmtksqlite_custom.so,libmtp.so,libmtp_ex.so,libmtpexif.so,libmtpip.so,libmttwebview_plat_support_rom.so,libmttwebview_rom.so,libmultidisplay.so,libmusi.so,libmutils.so,libMVersionCk.so,libmvmem.so,libmxkctl.so,libmxkspiflash.so,libmz_performance.so,libmztimedtext.so,libnda.so,libnanddatactrl.so,libnativebitmap.so,libnativebridge.so,libnative-bridge.so,libnativebridge_lazy.so,libnativecfms.so,libnative-core.so,libnative-cube.so,libnativedfrappeyehandler_jni.so,libnativedisplay.so,libNativeGraphicRender.so,libnativehcm.so,libnativehelper.so,libnativeloader.so,libnativeloader_lazy.so,libNativeMethods.so,libnative-platform.so,libnativeutils.so,libnative-vbridge.so,libnativewindow.so,libNavXtend.so,libNavXtend.so,libnb.so,libnbaio.so,libnblog.so,libndk_translation.so,libndk_translation_proxy_libandroid.so,libndk_translation_proxy_libandroid_runtime.so,libndk_translation_proxy_libc.so,libndk_translation_proxy_libEGL.so,libndk_translation_proxy_libGLESv.so,libndk_translation_proxy_libjnigraphics.so,libndk_translation_proxy_libmediandk.so,libndk_translation_proxy_libnativewindow.so,libndk_translation_proxy_libOpenSLES.so,libnemuVMprop.so,libneonuijni_public.so,libnetd_client.so,libnetdbpf.so,libnetdutils.so,libnetmgr.so,libnetutils.so,libneuralnetworks.so,libnexadaptation_layer.so,libnexaudiorenderer.so,libnexplayersdk.so,libnexstreaming_drm.so,libnexvideodisplayer.so,libnfc_device.so,libnfc_ndef.so,libnfc_uart_clsctl_ipc.so,libnfcmdl.so,libNgrHelper.so,libNimsWrap.so,libnl.so,libnlemv.so,libnlposapi.so,libnlsutils.so,libnms.so,libnode.so,libNoFpsActor.so,libnokaudiodec.so,libnopreloader.so,libnps.so,libnpt.so,libnsservice.so,libntf.so,libnubia_runtime.so,libnubiaconfig.so,libnv.so,libnv_public_interface.so,libnvblit.so,libnvcms.so,libnvcpl.so,libnvddk_d_v.so,libnvddk_vic.so,libnve.so,libnvglsi.so,libnvgr.so,libnvme.so,libnvopt_dvm.so,libnvos.so,libnvprodinfo.so,libnvram.so,libnvram_platform.so,libnvram_sec.so,libnvramagentclient.so,libnvrm.so,libnvrm_gpu.so,libnvrm_graphics.so,libnvrmapi_tegra.so,libNVUtils.so,libnvwsi.so,libnxutil.so,liboat.so,libodex_compress.so,libodispscene.so,liboem_jni.so,liboem_ssa.so,liboeminfo.so,liboeminfo_oem_api.so,liboemperf.so,libofflinecrypto.so,liboffline-decrypt.so,liboggextractor.so,liboithmb.so,libomadrm.so,libomafldrm.so,libomp.so,libomxcodec.so,libOmxCore.so,libOmxVppIf_product.so,liboncrpc.so,libonetrace_jni.so,libOniFile.so,libOolong.so,libopdiagnose.so,libOpenCL.so,libOpenCL_system.so,libOpenCLIcd.so,libOpenCv.so,libopencv_java.so,libopencv_java.so,libOpenglCodecCommon.so,libOpenglSystemCommon.so,libopenjdk.so,libopenjdkjvm.so,libopenjdkjvmti.so,libOpenMAXAL.so,libOpenNI.jni.so,libOpenNI.so,libOpenSLES.so,libopenssl.so,libopensslsmime.so,libopenvx.so,libopf.so,liboplus_bitHeif.so,liboplus_heifconverter.so,liboplus_imageprocessing.so,liboplus_multimedia_kernel_event.so,liboplusaudioDump.so,liboplusavenhancements.so,liboplusdrmdecoderjni.so,libOplusDualHeadPhoneEx.so,liboplusextzawgyi.so,libOplusGameVoiceOptimize.so,liboplusgraphic.so,liboplusgui_jni.so,liboplushwui_jni.so,liboplusloadframe.so,liboplusmediaplayerservice.so,liboplusmmdebug.so,liboplusmultimediaconfig.so,liboplusplugin.so,liboplusschedassisex.so,liboplussfplugin_ccodec.so,liboplusstagefright.so,liboplusstagefright_foundation.so,liboplustinyxml.so,liboplustpcomplogex.so,liboplusutils.so,liboppo_encryption.so,liboppo_imageprocessing.so,libOppoAudioVoiceScence.so,liboppoavenhancements.so,liboppodrmdecoderjni.so,liboppodrmutil.so,liboppoesservice.so,liboppoex.so,libOppoGameVoiceOptimize.so,liboppogift.so,liboppogift_service.so,liboppographic.so,liboppoheif.so,liboppoloadframe.so,liboppometadataretriever_jni.so,liboppoperf.so,liboppoplayer_jni.so,liboppostagefright.so,liboppowear_runtime.so,libopus.so,libopusTool.so,liborbbec.so,liborbbecusb.so,libOSUser.so,liboverlay.so,libovkey.so,libozoaudio.so,libozoaudiodecoder.so,libozoaudioencoder.so,libozoaudioutils.so,libozowidening.so,libpa.so,libpackagelistparser.so,libpad_hal.so,libpad_jni.so,libpadm.so,libpagemap.so,libpaho-mqtta.so,libpaho-mqttc.so,libpaladin.so,libpalwlan_mtk.so,libpanel.so,libpanelmode.so,libpara.so,libpatch.so,libpatchdex.so,libpatchelf.so,libpatcher.so,libpatrons.so,libpbo_jni.so,libpcas.so,libpcg.so,libpcgdvmjit.so,libpciaccess.so,libpcre.so,libpcre.so,libpcrecpp.so,libpcsclite.so,libpdfium.so,libpdx_default_transport.so,libperf.so,libperf_client.so,libperf_ctl.so,libperfctl.so,libperfetto_hprof.so,libperfframeinfo_jni.so,libperfhub_client.so,libperflog.so,libperfmanager.so,libperformance_jni.so,libperfservice.so,libperfservice_jni.so,libperfservicenative.so,libperftrace.so,libpermission.so,libpersona.so,libpf_customize_hisi.so,libpftrace.so,libpga.so,libPgaLogger.so,libphoenix_jni.so,libphoenix_native.so,libPhotoEnhance.so,libpicturemanager.so,libpieadapter.so,libpiex.so,libpimutex.so,libpipmanager.so,libpixelflinger.so,libpixelpipe.so,libPKCSWrapper.so,libpkg.so,libpla.so,lib-platform-compat-native-api.so,libplatforminfo.so,libpldbgutil.so,libplstorage.so,libpmfunc.so,libpng.so,libpng_private.so,libpnp-at.so,libpostfilter.so,libpostfilter_jni.so,libpower.so,libpowergenie_native.so,libpowerhal_cli.so,libpowerhalwrap.so,libpowerhalwrap_jni.so,libpowerlog.so,libpowermanager.so,libpowerservice_client.so,libPowerStretch.so,libPowerStretch.so,libPowerStretch.sprd.so,libpowervhal.so,libpq_cust.so,libpq_cust_base.so,libpq_cust_mtk.so,libpq_prot.so,libpq_prot_mtk.so,libpqhdrmetaparser.so,libpqservice.so,libpqsharememory.so,libpqtuning.so,libpre_install.so,libpredeflicker_native.so,libprelinker.so,libpreloader.so,libprivacystats.so,libprivatedata.so,libprocessgroup.so,libprocinfo.so,libprofile.so,libprofiledaemon.so,libprogrambinary.so,libprotecteyes.so,libprotobuf.so,libprotobuf-c.so,libprotobuf-c-plat.so,libprotobuf-cpp-full.so,libprotobuf-cpp-lite-...so,libprotobuf-cpp-lite.so,libProtoDB.so,libpsvgjhns.wdxj.so,libpthread.so,libpushinitzip.so,libpvrd.so,libpvrd_SGX_.so,libpvrANDROID_WSEGL.so,libpvrANDROID_WSEGL_SGX_.so,libPVRMtkutils.so,libPVROCL.so,libPVRRS.so,libpwavenhancements.so,libpwdclient.so,libqdtools_adreno.so,libqdtools_adreno.so,libqdtools_esx.so,libqueygtlcih.so,libqcci_legacy.so,libqchatptt.so,libqcomfm_if.so,libqcomfm_jni.so,libQcomUI.so,libqc-opt.so,libqct_resampler.so,libqdi.so,libqdMetaData.so,libqdMetaData.system.so,libqdutils.so,libqengine_core.so,libQjpeg.so,libqk_art.so,libqkbm_-v..so,libqksafetyspace.so,libqksscore.so,libQmageDecoder.so,libqmi.so,libqmi_cci.so,libqmi_cci_system.so,libqmi_client_helper.so,libqmi_client_qmux.so,libqmi_common_so.so,libqmi_dms_client.so,libqmi_encdec.so,libqmi_nv_api.so,libqmi_oem_api.so,libqminvapi.so,libqmiservices.so,libqrdec.so,libqrdinside.so,libqrengine.so,libqsap_sdk.so,libQSEEComAPI.so,libQseeOptService.so,libqseeproxy.so,libqservice.so,libqspmsvc.so,libqspower-...so,libqti_performance.so,libqti-at.so,libqti-gt.so,libqti-perfd-client.so,libqti-perfd-client_system.so,libqti-util.so,libqti-util_system.so,libqti-wl.so,libqueue.so,libquram.so,libquramagifencoder.so,libquramagifencoder_L.so,libquramimagecodec.so,libqvrcamera_client.so,libqvrservice_client.so,librac.so,libradio.so,libradio_metadata.so,libraw.so,libRBEGL_adreno.so,libRBGLESv_adreno.so,libRBGLESv_CM_adreno.so,librcm.so,libreactnativejni.so,libreactnativejnifb.so,libregionalization.so,libremotedesktop_client.knox.samsung.so,libremotedesktop_client.so,libremotedisplay.so,libremotedisplay_wfd.so,libRemoteEncoder.so,libres-load.so,libresmanager.so,libreverse_verify.so,librevh.so,librevh_baa.so,librfbinder-cpp.so,librga.so,libriemannplaid.so,librim.so,librk_demux.so,librk_on.so,librk_vpuapi.so,librkbm.so,libRoadLineRebuildAPI.so,librrc.so,libRS.so,librs_adreno.so,libRS_internal.so,librs_jni.so,librsa_hw.so,libRSCacheDir.so,libRScpp.so,libRSCpuRef.so,libRSDriver.so,libRSDriver_adreno.so,libRSDriver_mtk.so,libRSDriverArm.so,libRSDriverArm_v.so,librsmjni.so,librst.so,librtctest_jni.so,librtmp-jni.so,librtp_jni.so,librtspclient.so,libRtsSDK.so,libsdvgui.so,libsx.so,libsxd.so,libsaiv.so,libsaiv_BeautySolution.so,libsaiv_HprFace_FD_api.so,libsaiv_HprFace_FD_jni.so,libsaiv_HprFace_LD_api.so,libsaiv_HprFace_LD_jni.so,libsaiv_HprFace_utils_api.so,libsaiv_HprFace_utils_jni.so,libsaiv_nightshot.so,libsaiv_vision.so,libsamsung_symbols.so,libsamsungearcare.so,libsamsungeffect.so,libSamsungPkcsWrapper.so,libSamsungPostProcessConvertor.so,libsamsungSoundbooster_ext.so,libsamsungSoundbooster_pgb.so,libsamsungvad.so,libsandbox_.so,libsandhok.so,libsandhok-native.so,libsapporo.so,libsavsac.so,libsavscmn.so,libsavsff.so,libsavsmeta.so,libsavsvc.so,libsbgse.so,libsbs.so,libsbwcdecomp.so,libsbwchelper.so,libsc-axx.so,libscapi.so,libsccore.so,libschappy.so,libsched.so,libSchedAssistExtImpl.so,libSchedAssistJni.so,libschedulerservicehidl.so,libscheduling_policy.so,libscone_stub.so,libscore.so,libscript_core.so,libsdp.so,libsd_sdk_display.so,libSdemDebug.so,libSdemDebug.system.so,libsdi.so,libsdkpaplat.so,libSDKRecognitionText.so,libSDL.so,libsdm-disp-apis.so,libsdmutils.so,libsdp_crypto.so,libSDSDClient.so,libSDSDGame.so,libsdsprpc.so,libseams_binder.so,libSEC_EGL.so,libsec_km.so,libsec_mem.so,libsec_ode_km.so,libsec_ode_pbkdf.so,libsec_sdk...so,libsec_sdk.so,libsecaudio.so,libsecaudiocoreutils.so,libsecaudioinfo.so,libsecaudiotestutils.so,libSecDataMgr.so,libSecEmbms.so,libsechelper_engine.so,libsecimaging.so,libsecion.so,libsecjpeginterface.so,libSecLibJNI.so,libSecMMCodec.so,libsecnativefeature.so,libSecPedometer.so,libsecpkcs_engine.so,libsecril-client.so,libsecsdk.so,libsecuibc.so,libsecure.so,libsecure_storage.so,libsecure_storage_jni.so,libsecurec.so,libsecureui_svcsock.so,libSecureUILib.so,libsecuritycrypt.so,libsecuritydeviceserviceclient.so,libsecurityjni.so,libSecurityManagerNative.so,libsecuritysdk-...so,libsecuritysdkx-...so,libsecurityservice.so,libSeemplog.so,libSEFMP.so,libSEF.quram.so,libSEF.so,libsefinfoutils.so,libselinux.so,libselinux_ext.so,libSema.so,libsemcamera_jni.camera.samsung.so,libsemcamera_jni.so,libsensor.so,libsensor.so,libsensor_lge_cal.so,libsensor_low_lat.so,libsensor_reg.so,libsensor_test.so,libsensor_test.so,libsensor_thresh.so,libsensor_user_cal.so,libsensor_user_cal.so,libsensorextimpl.so,libsensorhub.so,libsensorhubservice_jni.so,libsensorndkbridge.so,libsensorservice.so,libsensorslog.so,libsepdrm.so,libserial_port.so,libserialport.so,libserialport_device.so,libservicedexloader.so,libservicelibloader.so,libservices.honor.so,libservices.huawei.so,libservices.magic.so,libservices.so,libserviceutility.so,libset.so,libsetinputUtil.so,libsf_cpupolicy.so,libsfextcp.so,libsfplugin_ccodec.so,libsfplugin_ccodec_utils.so,libsgavmpso-...so,libSgCoreCpt.so,libsgmainso-...so,libsgmiddletierso-...so,libsgmiscso-...so,libsgnocaptchaso-...so,libsgsecuritybodyso-...so,libsh.so,libsh_imagequalityadjust.so,libsharedbuffer.so,libshdisp.so,libshdisp_product.so,libshdovi.so,libshell.so,libshell_jni.so,libshfont.so,libshgpupm.so,libshim_atomic.so,libshmemcompat.so,libshmemutil.so,libsh-native.so,libshootdetect.so,libshthermal.so,libshtps.so,libshtpspm.so,libshtrpd.so,libshvpp_interface_product.so,libshwatch.so,libsigchain.so,libsilk_jni.so,libsim.so,libSipc.so,libsisodrm.so,libskeletoncore.so,libskewknob_system.so,libskia.so,libskia_jbmr.so,libskia_kkmr.so,libskia_mtk.so,libskia_neon_uc.so,libskia_opt.so,libskia_v.so,libskiaemoji.so,libskiafont.so,libskiaimagehw.so,libskiaoppo.so,libskiaso.so,libskjpegencoderextimpl.so,libskjpegturbo.so,libskmm.so,libSKT_MusicDRM.so,libsktdrm_sf.so,libsktdrm_sf_wrap.so,libsktdrmdevice.so,libsktdrminterface.so,libskuwa.so,libslpc.so,libsls_producer.so,libsmart_cropping.so,libsmartaudioservice.so,libsmartdl.so,libsmartfitting_interface.so,libsmartinstance_zygote.so,libsmartpolicy.so,libsmartshow.so,libsmd.so,libsmemlog.so,libsmmu_client.so,libsmsd.so,libSMTSensorCalLibNative.so,libsn_jni.so,libsnappy.so,libsns_low_lat_stream_stub.so,libSocialXtend.so,libSocialXtend.so,libsocket_binder.so,libsoftkeymasterdevice.so,libsoftsim_jni.so,libsomcavextensionsimpl.so,libsomcenhancements.so,libsomcmediahttp.so,libsomp.so,libsoname.so,libsonic.so,libsonivox.so,libsonydseehxwrapper.so,libSonyIMXPdafLibrary.so,libsoterclient.so,libsoundalive.so,libSoundAlive_SRC_vera.so,libSoundAlive_VSP_vera_ARMCpp.so,libSoundAlive_VSP_vera_ARMCpp_bit.so,libSoundAlive_VSP_verb_arm.so,libsoundextractor.so,libsoundpool.so,libsoundspeed.so,libsoundtouch.so,libsound-touch.so,libsoundtrigger.so,libsoxr.so,libsparse.so,libspc-native.so,libspeech_enh_lib.so,libspeexresampler.so,libsprdssense.so,libsqlite.so,libsqlite_expert.so,libsqlite_udr.so,libsqlite.so,libsqlite_lewa.so,libSRIBSE_Lib.so,libsrsprocessing.so,libsrsprocessing.so,libsrv.so,libsrv_um.so,libsrv_um_SGX_.so,libsrvcex.so,libssacmd.so,libssc.so,libssl.so,libssl_fips.so,libsslengine.so,libssv_androidfw.so,libsta.so,libstagefright.so,libstagefright_amrnb_common.so,libstagefright_avc_common.so,libstagefright_bufferpool@...so,libstagefright_bufferpool@..so,libstagefright_bufferqueue_helper.so,libstagefright_codecbase.so,libstagefright_data.so,libstagefright_enc_common.so,libstagefright_flacdec.so,libstagefright_foundation.so,libstagefright_framecapture_utils.so,libstagefright_http_support.so,libstagefright_httplive.so,libstagefright_hwextendedmediadrm.so,libstagefright_idd.so,libstagefright_LGdmb.so,libstagefright_lgdmbcolorconversion.so,libstagefright_local_cache.so,libstagefright_memutil.so,libstagefright_mlb_cache.so,libstagefright_mtvnode.so,libstagefright_nts.so,libstagefright_omx.so,libstagefright_omx_utils.so,libstagefright_ozoenc.so,libstagefright_shim.so,libstagefright_soft_aacenc.so,libstagefright_soft_avcdec.so,libstagefright_soft_eglenc.so,libstagefright_soft_ffmpeg.so,libstagefright_soft_glenc.so,libstagefright_soft_mpdec.so,libstagefright_soft_plugin.so,libstagefright_strm_snooper.so,libstagefright_surface_utils.so,libstagefright_timedtext_ccparser.so,libstagefright_timedtext_xsub.so,libstagefright_usbd.so,libstagefright_wfd.so,libstagefright_wfd_mtk.so,libstagefright_wfd_oplus.so,libstagefright_xmlparser.so,libstagefright_xmlparser@..so,libstagefright_yuv.so,libstagefrighthw.so,libstagefrighthw_cm.so,libstagefrighthw_yuv.so,libstartup_log.so,libstartup_log_jni.so,libstats_jni.so,libstatslog.so,libstatspull.so,libstatssocket.so,libstdc++.so,libstdc++_real.so,libStDrvInt.so,libstf.so,libstface_api.so,libsthmb.so,libstidfacepro_api.so,libstlport.so,libstlport_shared.so,libstreamingpolicy.so,libsts_values.so,libStSerialPort.so,libsuda-phoneloc-jni.so,libsudautils.so,libsuntory_utils.so,libSuppressionAreaConfiguration.so,libsurfaceflinger.so,libsurfaceflinger_client.so,libSurfaceFlingerProp.so,libsurfaceutil.so,libsuspend.so,libswa.so,libswapro.so,libswconfig.so,libsweffectwrapper.so,libSwJpgCodec.so,libswresample.so,libswscale.so,libswsprocessing.so,libsXehhhS.so,libsxqk.so,libsxqk_skia.so,libsync.so,libsysmgrapi.so,libsystem_ext.so,libsystem_ext_common.so,libsysutils.so,libtaglib.so,libtailor.so,libtair.so,libtair_bba.so,libtango_hal.so,libtango_hardware.so,libtango_surface_texture.so,libtcb.so,libtcore_teec_api.so,libtcpfinaggr.so,libtcpinfo.so,libTcpOptimizer.mobiledata.samsung.so,libTcpOptimizer.so,libtdutils.so,libteec.honor.so,libteec.huawei.so,libteec.so,libteecl.so,libteeclient.so,libTEEClient.so,libteeclientjni.so,libteejni.so,libteeserver.so,libTEEService.so,libtextclassifier.so,libtextclassifier_hash.so,libtfa.so,libtfa_interface.so,libtfaxx.so,libtfaxx_hal.so,libtfaspeaker.so,libtflite.so,libtg_speech_msg.so,libthai.so,libthemeutils_jni.so,libthermalclient.so,libthr.so,libthreedimensionmanager.so,libThumbCompensation.so,libthwsplit.so,libtiff.so,libtiff_directory.so,libTigerService_binder.so,libtilerenderer.so,libtime_genoff.so,libtimeinstate.so,libtimestretch.so,libtin.so,libtinyalsa.so,libtinyxml.so,libtinyxml.so,libtinyxmlbp.so,libtlc_comm.so,libtlc_direct_comm.so,libtlc_proxy.so,libtlc_proxy_comm.so,libtlc_proxy_tui.so,libtlc_tima_tui.so,libtlc_tz_ccm.so,libtlc_tz_dcm.so,libtmdisplay.so,libtms.so,libtneclient.so,libtnet-...so,libtnet-..bk.so,libtobEmbedEncrypt.so,libtombstoned_client.so,libtool.so,libTorch.so,libtouchengine.so,libtouchfilter.so,libtoyger.so,libtoyger_doc.so,libtpm.so,libtqvalidate.so,libtran_resmonitor.so,libtranlog.so,libtranslator_check_stillframe_v.so,libtranslator_textfusion_v.so,libtrapz.so,libtscheckswitch.so,libtsdb-jni.so,libtsec_wrapper.so,libtsparser.so,libtsparser_LDTV.so,libtspower.so,libtsSDK.so,libTTArtArm.so,libTTArtArm.so,libtts_skeleton.so,libtun.so,libtunefs.so,libturingau.bafbc.so,libturingau.ca.so,libturingau.cb.so,libturingau.deac.so,libturingau.ebea.so,libturingau.bce.so,libturingau.cbdce.so,libturingau.cdf.so,libturingau.fbcc.so,libtvmanager.so,libTVOut.so,libtvoutinterface.so,libtvoutservice.so,libtxc_dxtn.so,libtz_uree.so,libtzcom.so,libuplayer.so,libuc_parser_ac.so,libuc_parser_asf.so,libuc_parser_avi.so,libuc_parser_dts.so,libuc_parser_flv.so,libuc_parser_mkv.so,libuc_parser_ogm.so,libuc_parser_qcp.so,libucalmond.so,libucaudioutils.so,libucdrm.so,libucftmitem.so,libucmabl.so,libucresampler.so,libucRmsVolComp.so,libucsdk_...._libSLASHlibhomodisablerDOTso.so,libucsdk_...._libSLASHlibjsiDOTso.so,libucsdk_...._libSLASHlibwebviewucDOTso.so,libucsecclk.so,libucsengine.so,libudf.so,libudrupkjg.ritp.so,libufs.so,libufwriter.so,libuhab.so,libui.so,libui_ext.so,libui_ext_fwk.so,libui_shim.so,libuicc.so,libuimmdl.so,libuiturbo.so,libulcaudioparam.so,libumeng-spy.so,libUMP.so,libunc_divxdrm.so,libunc_soundnormalizerV.so,libunctars.so,libunwind.so,libunwind-ptrace.so,libunwindstack.so,libUpdateUtils.so,libuplayer-...so,liburee_ca.so,libusb.so,libusb.so,libusbhost.so,libusc.so,libusc_SGX_.so,libut_c_api.so,libutilitysw.so,libutils.so,libutils.z.so,libutils_jni.z.so,libutilscallstack.so,libutopia.so,libuusafe.so,libuvc.so,libUVCCamera.so,libUVCCameraHelper.so,libv.so,libV_UC.so,libvjsbridge.so,libvjsbridge_jsi.so,libvlc.so,libvtc.so,libvuc.so,libvworker-native.so,libv++.so,libva++.so,libva.so,libva_videodecoder.so,libva_videoencoder.so,libva++.so,libva-android.so,libval_..so,libval_common.so,libval_mixer.so,libva-tpi.so,libvb.so,libvboxa.so,libvboxd.so,libvcdec_sa.ca.so,libvchiq.so,libvcm.so,libvcode_nc.so,libvcodec_oal.so,libvcodec_utility.so,libvcodecdrv.so,libvcos.so,libvcsm.so,libvdecoderserviceclient.so,libvendorcfg.so,libvendorconn.so,libvendorutils.so,libVGT.so,libvibrator.so,libvicom.so,libvicom_conf.so,libvicom_engine.so,libvicom_interface.so,libvicom_recon.so,libvicom_vendor.so,libvideodecryptphone.so,libvideodecrypt.so,libvideoeditor_core.so,libvideoeditor_jni.so,libvideoeditor_jni_N_za.so,libvideoeditor_jni_N_zf.so,libvideoeditor_N_za.so,libvideoeditor_N_zf.so,libvideoeditor_osal.so,libvideoeditor_videofilters.so,libvideoeditorplayer.so,libvideoinfo.so,libvideoset.so,libvintf.so,libvirtualbus.so,libvirtualdevices.so,libvirtualdisplay.so,libvirtualdisplaymodule.so,libvirus.so,libvivo_runtime.so,libvivo_sensor_runtime.so,libvivoaudiopolicymanager.so,libvivofmkmedialogctrl.so,libvivo-vperfd-client_system.so,libvixl.so,libvixl-arm.so,libvixl-arm.so,libVmiCJSON.so,libVmiDevice.so,libVmiMediaCommon.so,libVmiOperation.so,libVmiShareMem.so,libVmiStream.so,libvmkid_clibs.so,libvmkid_lemur.so,libvmmem.so,libvms.so,libvndksupport.so,libvoAACDec.so,libvobjectlib.so,libvoice_judge.so,libVoiceCrypt.so,libVoiceScenceDiscern.so,libvolumevibratorcallback.so,libvorbisidec.so,libvpdec_sa.ca.so,libvpenc_sa.ca.so,libvphone.so,libvpp_setting.so,libvppvideobox.so,libvpsextension.so,libvptwrapper.so,libvpu.so,libVR.so,libvr_amb_engine.so,libvr_object_engine.so,libvr_sam_wrapper.so,libvraudio.so,libvraudio_client.so,libVSC.so,libvsyncbridge.so,libVT.so,libvulkan.so,libvulkan_enc.so,libvulkan_sec.so,libVulkanContext.so,libwalle_base.so,libwatchdog.so,libwavextractor.so,libwayland-client.so,libwbktjqbf.monv.so,libwebcore.so,libWebCore_UC.so,libwebkit_glcanvas.so,libwebp_private.so,libwebviewchromium.huawei.so,libwebviewchromium.so,libwebviewchromium_aosp.so,libwebviewchromium_loader.so,libwebviewchromium_plat_support.huawei.so,libwebviewchromium_plat_support.so,libwebviewcl.so,libwebviewextchromium.so,libwebviewuc.so,libWebXtend.so,libWebXtend.so,libweexcore.so,libweexv.so,libweexvbk.so,libWFC.so,libwfcu.so,libwfdmmservice.so,libwgsgcj.so,libwhale.edxp.so,libwiegand_device.so,libwifilogevent.so,libwilhelm.so,libwind.so,libwnndict.so,libWnnEngDic.so,libWnnJpnDic.so,libwpa_client.so,libwvdrm_L.so,libwvm.so,libWVStreamControlAPI_L.so,libwxvoiceembed.so,libx.so,libX.so,libxBlas.so,libxcoder.so,libxcollie.so,libxheif.so,libxInline.so,libXJwz.rM.so,libxlog.so,libxmaudio.so,libxmcnano.so,libxmcore.so,libxml.so,libxml.so,libxms_loader.so,libxNano.so,libxnn.so,libxnnloader.so,libxnns.so,libxnnvplugin_jsi.so,libxp_ee_kit_core.so,libxpcamera_client.so,libxpcarapi.so,libxriver-android.so,libxriver-ccdn-adapter.so,libxriver-classloader.so,libxriver-core.so,libxriver-jsi-adapter.so,libyaqbasic.bafbc.so,libyaqbasic.ca.so,libyaqbasic.cb.so,libyaqbasic.deac.so,libyaqbasic.ebea.so,libyaqbasic.bce.so,libyaqbasic.cbdce.so,libyaqbasic.cdf.so,libyaqbasic.fbcc.so,libyaqpro.bafbc.so,libyaqpro.ca.so,libyaqpro.cb.so,libyaqpro.deac.so,libyaqpro.ebea.so,libyaqpro.bce.so,libyaqpro.cbdce.so,libyaqpro.cdf.so,libyaqpro.fbcc.so,libymcmach.so,libyoga.so,libyotadevices_handdetection.so,libyuv.so,libyuv_static.so,libyuv_transform.so,libz.so,libzface.so,libziparchive.so,libzive.so,libzive_c.so,libzmvmpcls.so,libzstd.so,libzstd_decode.so,libztcodec.so,libzteavenhancements.so,libztemediaextractor.so,libzteSmartBeauty.so,libzteSmartBeauty_Arm.so,libzuma.so,libzuma.so,libzxves.so,libZytCrptManager.so,media_permission-aidl-cpp.so,mediametricsservice-aidl-cpp.so,memtrack.amlogic.so,memtrack.apq.so,memtrack.exynos.so,memtrack.gmin.so,memtrack.hi.so,memtrack.hioem.so,memtrack.hisft.so,memtrack.mrvl.so,memtrack.msm.so,memtrack.mt.so,memtrack.mtm.so,memtrack.mtt.so,memtrack.rk.so,memtrack.sc.so,memtrack.sdm.so,memtrack.so,memtrack.suniwp.so,memtrack.tegra.so,memtrack.universal.so,mi_display.so,OpenGLES.so,packagemanager_aidl-cpp.so,perfboost.mt.so,perfservice_aidl_interface-V-cpp.so,pp_proc_plugin.so,QXProfiler.so,runtime.so,sc-axx.so,server_configurable_flags.so,shared-file-region-aidl-cpp.so,spatializer-aidl-cpp.so,spl_proc_plugin.so,stamina.so,tcp-connections.so,utils.so,vendor.bbry.hardware.bide@..so,vendor.delta.hardware.dperf@..so,vendor.display.color@..so,vendor.display.postproc@..so,vendor.dolby.hardware.dms@..so,vendor.hardware.setinput@..so,vendor.honor.hardware.camera.camResource@..so,vendor.honor.hardware.graphics.common@..so,vendor.honor.hardware.honorsigntool@..so,vendor.honor.hardware.hwdisplay@..so,vendor.honor.hardware.hwfactoryinterface@..so,vendor.honor.hardware.hwsched@..so,vendor.honor.hardware.iawareperf@..so,vendor.honor.hardware.jpegdec@..so,vendor.honor.hardware.motion@..so,vendor.honor.hardware.pixelworks_display@..so,vendor.honor.hardware.pixelworks_feature@..so,vendor.honor.hardware.sensors@..so,vendor.honor.hardware.snpelaunch@..so,vendor.honor.hardware.tp@..so,vendor.huawei.hardware.ai@..so,vendor.huawei.hardware.biometrics.hwfacerecognize@..so,vendor.huawei.hardware.camera.camResource@..so,vendor.huawei.hardware.graphics.common@..so,vendor.huawei.hardware.graphics.displayeffect@..so,vendor.huawei.hardware.graphics.gpucommon@..so,vendor.huawei.hardware.graphics.mediacomm@..so,vendor.huawei.hardware.huaweisigntool@..so,vendor.huawei.hardware.hwdisplay@..so,vendor.huawei.hardware.hwfactoryinterface@..so,vendor.huawei.hardware.hwsched@..so,vendor.huawei.hardware.iawareperf@..so,vendor.huawei.hardware.jpegdec@..so,vendor.huawei.hardware.libteec@..so,vendor.huawei.hardware.motion@..so,vendor.huawei.hardware.sensors@..so,vendor.huawei.hardware.tp@..so,vendor.huawei.hardware.vdecoder@..so,vendor.hw.memtrack@..so,vendor.hw.vsystem@..so,vendor.hw.vsystems@..so,vendor.hw.vsystemw@..so,vendor.hw.xcamera@..so,vendor.hw.xsensors@..so,vendor.hx.hardware.tcb@..so,vendor.jlq.hardware.iop@..so,vendor.jlq.hardware.perf@..so,vendor.lge.hardware.configstore@..so,vendor.lge.hardware.configstore-utils.so,vendor.lge.hardware.ftmitem@..so,vendor.lge.hardware.powerhint.rescontrol@..so,vendor.lge.hardware.property@..so,vendor.lineage.power-V-cpp.so,vendor.mediatek.hardware.camera.camportal@..so,vendor.mediatek.hardware.gpu@..so,vendor.mediatek.hardware.mmagent@..so,vendor.mediatek.hardware.mms@..so,vendor.mediatek.hardware.mms@._vendor.so,vendor.mediatek.hardware.mtkpower@..so,vendor.mediatek.hardware.power@..so,vendor.mediatek.hardware.power@._vendor.so,vendor.mediatek.hardware.pq@..so,vendor.mediatek.hardware.pq@._vendor.so,vendor.mediatek.hardware.pq_aidl-V-ndk.so,vendor.mediatek.hardware.qt@..so,vendor.oplus.hardware.performance@..so,vendor.oppo.hardware.gift@..so,vendor.oppo.hardware.hypnus@..so,vendor.qti.hardware.audiohalext@..so,vendor.qti.hardware.audiohalext-utils.so,vendor.qti.hardware.display.mapper@..so,vendor.qti.hardware.display.mapperextensions@..so,vendor.qti.hardware.iop@..so,vendor.qti.hardware.ListenSoundModel@..so,vendor.qti.hardware.perf@..so,vendor.qti.hardware.perf@._vendor.so,vendor.qti.hardware.vpp@..so,vendor.qti.qspmhal@..so,vendor.samsung.external.secure_storage@..so,vendor.samsung.hardware.hyper-V-cpp.so,vendor.samsung.hardware.miscpower@..so,vendor.samsung.hardware.security.proca@..so,vendor.samsung.hardware.security.securestorage@..so,vendor.samsung.security.proca@..so,vendor.samsung_slsi.hardware.configstore@..so,vendor.samsung_slsi.hardware.configstore-utils.so,vendor.semc.hardware.drm@..so,vendor.semc.system.idd@..so,vendor.sharp.hardware.appnotifier@..so,vendor.sharp.hardware.display@..so,vendor.sharp.hardware.shdisp@..so,vendor.sharp.hardware.shvppmanager@..so,vendor.sharp.hardware.touchpanel@..so,vendor.so,vendor.sprd.hardware.power@..so,vendor.sprd.hardware.radio@..so,vendor.vivo.hardware.configstore@..so,vendor.vivo.hardware.vperf@..so,vintf-codecsolution-V-cpp.so,virtio_gpu_dri.so,vmwgfx_dri.so,vulkan.adreno.so,vulkan.gs.so,vulkan.mali.so,vulkan.msm.so,vulkan.samsung.so,vulkan.sdm.so";

#define RESULT_CELL_FORMAT      "%s#%s^"
#define RESULT_CELL_FORMAT_INT      "%s#%d^"

#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "DVM-darren", __VA_ARGS__))

#define LOG_JNI_EXCEPTION(env, clear) { env->ExceptionDescribe(); if(clear)env->ExceptionClear(); }

#define ALIPAY_APP "com/alipay/mobile/framework/AlipayApplication"

#define ALIPAY_APP_pplicationAgent "com/alipay/mobile/framework/LauncherApplicationAgent"

#define ALIPAY_APP_pplicationAContext  "com/alipay/mobile/framework/MicroApplicationContext"

#define ALIPAY_APP_AUService "com/alipay/mobile/framework/service/ext/security/AuthService"

#define ALIPAY_APP_Userinfo  "com/alipay/mobile/framework/service/ext/security/bean/UserInfo"



#define BINARY_UNIT_SIZE 3
#define BASE64_UNIT_SIZE 4
#define MAX_LENGTH 256

int dvm_entry(int argc, char **argv);

int  deleteNumber(const char *str, char *dst)
{
    if (!str | !dst)
    {
        return -1;
    }
    int a = strlen(str);
    int i = 0;
    for (i  = 0; i < a; i++)
    {
        if (*str == '\n' || *str == '\r' ||  *str == '0' || *str == '1' ||
            *str == '2' || *str == '3' || *str == '4' || *str == '5' || *str == '6'
            || *str == '7' || *str == '8' || *str == '9'
                ){
            str++;
        }else{
            *dst++ = *str++;
        }
    }
    return 0;
}

int is_end_with(const char *str1, const char *str2)
{
    if(str1 == NULL || str2 == NULL)
        return -1;
    int len1 = strlen(str1);
    int len2 = strlen(str2);
    if((len1 < len2) ||  (len1 == 0 || len2 == 0))
        return -1;
    while(len2 >= 1)
    {
        if(str2[len2 - 1] != str1[len1 - 1])
            return 0;
        len2--;
        len1--;
    }
    return 1;
}

static jstring getClsName(JNIEnv *env, jclass myCls){

    jclass  objcls = NULL;
    jmethodID mid = NULL;
    jstring strObj= NULL;
    int ret = 0;
    do{
        objcls = env->FindClass("java/lang/Class");
        if(NULL == objcls || env->ExceptionCheck()){
            ret = -1;
            break;
        }
        mid = env->GetMethodID(objcls, "getName", "()Ljava/lang/String;");
        if(NULL == mid || env->ExceptionCheck()){
            ret = -2;
            break;
        }
        strObj = (jstring)env->CallObjectMethod(myCls, mid);
        if(NULL == strObj || env->ExceptionCheck()){
            ret = -3;
            break;
        }
    } while (0);
    if (objcls) env->DeleteLocalRef(objcls);


    return strObj;
}

static bool  checkException(JNIEnv *env)
{
    if (NULL == env)
    {
        return true;
    }
    if (env->ExceptionCheck())
    {
        env->ExceptionDescribe();
        env->ExceptionClear();
        return true;
    }
    jthrowable err = env->ExceptionOccurred();
    if (err != NULL){
        env->ExceptionClear();
    }
    return false;
}

static int  get_uid_from_alipay(JNIEnv *env, jobject  thiz, char* content, int len) {

    jclass classClazz = NULL;
    jclass alipayAppClazz =NULL;
    jmethodID loaderMethod = NULL;
    jmethodID getInstanceloadMethod = NULL;
    jobject localLoader = NULL;
    jobject getInstanceObj = NULL;

    jclass  launcAppContextClass = NULL;
    jmethodID getMicroApplicationContextMethod = NULL;
    jobject getMicroApplicationContextObj = NULL;

    jclass  launcherAppAgentClass = NULL;
    jmethodID findServiceByInterfacetMethod = NULL;
    jobject findServiceByInterfaceObj = NULL;

    jclass microApplicationContextClass = NULL;


    jclass auServiceClass = NULL;
    jmethodID getUserInfoMethod = NULL;
    jobject userinfoObj = NULL;


    jclass userinfoClass = NULL;
    jmethodID getUserIdMethod = NULL;
    jstring useridObj = NULL;


    jstring authServiceJtring = NULL;
    char* userId = NULL;

    int  ret = 0;
    do {
        classClazz =env->GetObjectClass(thiz);
        if(NULL == classClazz || env->ExceptionCheck()){
            ret = -1;
            break;
        }
        loaderMethod = env->GetMethodID(classClazz, "getClassLoader", "()Ljava/lang/ClassLoader;");
        if(NULL == loaderMethod || env->ExceptionCheck()){
            ret = -2;
            break;
        }
        localLoader = env->CallObjectMethod(thiz, loaderMethod);
        if(NULL == localLoader || env->ExceptionCheck()){
            ret = -3;
            break;
        }
        jclass loaderClass = env->GetObjectClass(localLoader);
        if(checkException(env) || !loaderClass) {
            ret = -4;
            break;
        }
        jmethodID  m = env->GetMethodID(loaderClass, "loadClass",
                                        "(Ljava/lang/String;)Ljava/lang/Class;");
        if(checkException(env) || !m) {
            ret = -5;
            break;
        }
        alipayAppClazz = (jclass)env->CallObjectMethod(localLoader, m,env->NewStringUTF(ALIPAY_APP));
        if(checkException(env) || !alipayAppClazz){
            ret = -6;
            break;
        }
        getInstanceloadMethod =env->GetStaticMethodID(alipayAppClazz, "getInstance", "()Lcom/alipay/mobile/framework/AlipayApplication;");
        if(getInstanceloadMethod == NULL ||  checkException(env)){
            ret = -7;
            break;
        }
        jvalue * Args = (jvalue *) malloc(sizeof(jvalue) * 1);
        getInstanceObj =(jobject)env->CallStaticObjectMethodA(alipayAppClazz, getInstanceloadMethod, Args);
        if(getInstanceObj == NULL || checkException(env)){
            ret = -8;
            break;
        }
        launcherAppAgentClass = (jclass)env->CallObjectMethod(localLoader, m,env->NewStringUTF(ALIPAY_APP_pplicationAgent));
        if(checkException(env) || !launcherAppAgentClass){
            ret = -9;
            break;
        }

        getMicroApplicationContextMethod =env->GetMethodID(launcherAppAgentClass, "getMicroApplicationContext", "()Lcom/alipay/mobile/framework/MicroApplicationContext;");
        if(getInstanceloadMethod == NULL ||  checkException(env)){
            ret = -10;
            break;
        }
        jvalue * Args1 = (jvalue *) malloc(sizeof(jvalue) * 1);
        getMicroApplicationContextObj =(jobject)env->CallObjectMethodA(getInstanceObj, getMicroApplicationContextMethod, Args1);
        if(getInstanceObj == NULL || checkException(env)){
            ret = -11;
            break;
        }
        microApplicationContextClass = (jclass)env->CallObjectMethod(localLoader, m,env->NewStringUTF(ALIPAY_APP_pplicationAContext));
        if(checkException(env) || !launcherAppAgentClass){
            ret = -12;
            break;
        }
        findServiceByInterfacetMethod =env->GetMethodID(microApplicationContextClass, "getExtServiceByInterface", "(Ljava/lang/String;)Lcom/alipay/mobile/framework/service/ext/ExternalService;");
        if(findServiceByInterfacetMethod == NULL ||  checkException(env)){
            ret = -13;
            break;
        }

        auServiceClass = (jclass)env->CallObjectMethod(localLoader, m,env->NewStringUTF(ALIPAY_APP_AUService));
        if(checkException(env) || !auServiceClass){
            ret = -14;
            break;
        }
        authServiceJtring = getClsName(env, auServiceClass);
        if(checkException(env) || !authServiceJtring){
            ret = -14;
            break;
        }

        jvalue * Args2 = (jvalue *) malloc(sizeof(jvalue) * 1);
        Args2[0].l =(jobject)authServiceJtring;
        findServiceByInterfaceObj =(jobject)env->CallObjectMethodA(getMicroApplicationContextObj, findServiceByInterfacetMethod, Args2);
        if(findServiceByInterfaceObj == NULL || checkException(env)){
            ret = -14;
            break;
        }
        getUserInfoMethod =env->GetMethodID(auServiceClass, "getUserInfo", "()Lcom/alipay/mobile/framework/service/ext/security/bean/UserInfo;");
        if (getUserInfoMethod == NULL)
        {
            getUserInfoMethod =env->GetMethodID(auServiceClass, "getLastLoginedUserInfo", "()Lcom/alipay/mobile/framework/service/ext/security/bean/UserInfo;");
        }
        if(getUserInfoMethod == NULL ||  checkException(env)){
            ret = -16;
            break;
        }
        userinfoObj =(jobject)env->CallObjectMethod(findServiceByInterfaceObj, getUserInfoMethod);
        if(userinfoObj == NULL || checkException(env)){
            ret = -17;
            break;
        }

        userinfoClass = (jclass)env->CallObjectMethod(localLoader, m,env->NewStringUTF(ALIPAY_APP_Userinfo));
        if(checkException(env) || !userinfoClass){
            ret = -18;
            break;
        }

        getUserIdMethod =env->GetMethodID(userinfoClass, "getUserId", "()Ljava/lang/String;");
        if(getUserIdMethod == NULL ||  checkException(env)){
            ret = -19;
            break;
        }
        useridObj =(jstring)env->CallObjectMethod(userinfoObj, getUserIdMethod);
        if(useridObj == NULL || checkException(env)){
            ret = -20;
            break;
        }
        userId = (char*)env->GetStringUTFChars(useridObj, 0);
        if(userId == NULL){
            ret = -21;
            break;
        }
        strncpy(content, userId, len);
    } while (0);
    LOGE("upload_files_for_path call  ret:%d, %s\n", ret, content);

    env->ReleaseStringUTFChars(useridObj, userId);
    if (classClazz) env->DeleteLocalRef(classClazz);
    if (userinfoClass) env->DeleteLocalRef(userinfoClass);
    if (auServiceClass) env->DeleteLocalRef(auServiceClass);

    if (microApplicationContextClass) env->DeleteLocalRef(microApplicationContextClass);
    if (launcherAppAgentClass) env->DeleteLocalRef(launcherAppAgentClass);

    if (alipayAppClazz) env->DeleteLocalRef(alipayAppClazz);
    if (launcAppContextClass) env->DeleteLocalRef(launcAppContextClass);

    // if (useridObj) env->DeleteLocalRef(useridObj);
    // if (findServiceByInterfaceObj) env->DeleteLocalRef(findServiceByInterfaceObj);
    // if (getMicroApplicationContextObj) env->DeleteLocalRef(getMicroApplicationContextObj);
    // if (getInstanceObj) env->DeleteLocalRef(getInstanceObj);
    // if (localLoader) env->DeleteLocalRef(localLoader);
    // if (userinfoObj) env->DeleteLocalRef(userinfoObj);
    // if (authServiceJtring) env->DeleteLocalRef(authServiceJtring);
    if (env->ExceptionCheck()) {
        LOG_JNI_EXCEPTION(env, true);
    }
    return ret;
}

static unsigned int get_black_so_dex_info(JNIEnv *env,jobject ctx,char* destination, size_t total) {
    unsigned int ret = 0;
    FILE *fp;
    char* destination_p = destination;
    do {
        if ((fp = fopen("/proc/self/maps", "r")) == NULL) {
            LOGE("######### /proc/self/maps open  failed! #########");
            ret = -1;
            break;
        }
        int soCount  = 0;
        int NoWhiteCount = 0;
        int mapsCount = 0;

        char buff[MAX_LENGTH]={0};
        while (fgets(buff, sizeof(buff), fp)) {
            ++mapsCount;
            char module[MAX_LENGTH] = {0};
            char permission[8] = {0};
            int count = sscanf(buff, "%*llx-%*llx %s %*s %*s %*s %s",permission, module);
            if (count != 2 || strlen(module) <= 0) {
                continue;
            }
            if (is_end_with(module, ".so") == 1 && strstr(permission, "x")){
                char* soName;
                char rest[MAX_LENGTH] = {0};
                strncpy(rest, module, strlen(module));
                char*tmp = rest;
                while ((soName = strtok_r(tmp, "/", &tmp))){
                    if (strstr(soName, ".so")){
                        break;
                    }
                }
                ++soCount;
                char soNameNew[MAX_LENGTH]={0};
                deleteNumber(soName, soNameNew);
                if (!strstr(soWhiteStrDest, soNameNew) && strlen(permission) != 0 && strlen(module) != 0 ){
                    if (strlen(destination_p) <  total-100){
                        int len = strlen(permission)+ strlen(module)+3;
                        snprintf(destination, len, ";%s,%s", permission,module);
                        ++NoWhiteCount;
                        destination+=len-1;
                    }
                }
            }
            if (strstr(permission, "xp") && (is_end_with(module, ".apk") == 1 ||
                                             is_end_with(module, ".odex") == 1 || is_end_with(module, ".dex") == 1 ||
                                             is_end_with(module, ".vdex") == 1)){
                if (strlen(destination_p) <  total-100){
                    int len = strlen(permission)+ strlen(module)+3;
                    snprintf(destination, len, ";%s,%s", permission,module);
                    ++NoWhiteCount;
                    destination+=len-1;
                }
            }
        }
        if (strlen(destination_p) > total -100){
            destination -= 100;
        }
        LOGE("[+] DVM:=maps==%s, :%d", destination_p,  strlen(destination_p));
        char tmp[60];
        int errCode = get_uid_from_alipay(env, ctx, tmp, 60);
        if(errCode < 0){
            snprintf(destination, 60, ";ALL:%d,SO:%d,WH:%d;UID:%d", mapsCount, soCount,NoWhiteCount,errCode);
        }else{
            snprintf(destination, 60, ";ALL:%d,SO:%d,WH:%d;UID:%s", mapsCount, soCount,NoWhiteCount,tmp);
        }
        LOGE("[+] DVM:%d, %d, %d, %d", strlen(destination_p), mapsCount, soCount, NoWhiteCount);
    } while (0);
    fclose(fp);
    return ret;
}

char* getDvmBlackSoAndUid(JNIEnv *env,jobject ctx,int* error){


    const char* key = "dvm_black_so_dex_uid";
    int total =8000; //256*30+40 ~~ 7720
    int errCode = 0;
    char* destination = NULL;
    LOGE("destination getDvmBlackSoAndUid >>>");
    do
    {
        destination = (char*)malloc(total);
        if (NULL == destination) {
            LOGE("destination malloc err");
            errCode = -3;
            break;
        }
        memset(destination, 0, total);
        errCode= get_black_so_dex_info(env,ctx,destination, total);
    } while (0);
    *error = errCode;
    return destination;
}

