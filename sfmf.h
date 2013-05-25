#pragma once
// Media Foundation headers
//#include <d3d9.h>
#include <mfapi.h>
#include <mfidl.h>
#include <evr.h>
#include <Mfreadwrite.h>
#include <mferror.h>
#include <Mfsharingengine.h>
#include <Wmcodecdsp.h>
#include <Mfmediacapture.h>
#include <Wmcontainer.h>
#include <Mfcaptureengine.h>
#include <Mfmp2dlna.h>

// Media Foundation Smart Pointers

_WRL_PTR_TYPEDEF(IAdvancedMediaCapture);//Enables advanced media capture.
_WRL_PTR_TYPEDEF(IAdvancedMediaCaptureInitializationSettings);// Provides initialization settings for advanced media capture.
_WRL_PTR_TYPEDEF(IAdvancedMediaCaptureSettings);//Provides settings for advanced media capture.
//_WRL_PTR_TYPEDEF(IDirect3DDeviceManager9);//Enables two threads to share the same Direct3D 9 device, and provides access to the DirectX Video Acceleration (DXVA) features of the device.
//_WRL_PTR_TYPEDEF(IDirectXVideoAccelerationService);//Provides DirectX Video Acceleration (DXVA) services from a Direct3D device.
//_WRL_PTR_TYPEDEF(IDirectXVideoDecoder);//Represents a DirectX Video Acceleration (DXVA) video decoder device.
//_WRL_PTR_TYPEDEF(IDirectXVideoDecoderService);//Provides access to DirectX Video Acceleration (DXVA) decoder services.
//_WRL_PTR_TYPEDEF(IDirectXVideoMemoryConfiguration);//Sets the type of video memory for uncompressed video surfaces.
//_WRL_PTR_TYPEDEF(IDirectXVideoProcessor);//Represents a DirectX Video Acceleration (DXVA) video processor device.
//_WRL_PTR_TYPEDEF(IDirectXVideoProcessorService);//Provides access to DirectX Video Acceleration (DXVA) video processing services.
_WRL_PTR_TYPEDEF(IEVRFilterConfig);//Sets the number of input pins on the DirectShow Enhanced Video Renderer (EVR) filter.
_WRL_PTR_TYPEDEF(IEVRFilterConfigEx);//Configures the DirectShow Enhanced Video Renderer (EVR) filter.
_WRL_PTR_TYPEDEF(IEVRTrustedVideoPlugin);//Enables a plug-in component for the enhanced video renderer (EVR) to work with protected media.
//_WRL_PTR_TYPEDEF(IEVRVideoStreamControl);//This interface is not supported.
_WRL_PTR_TYPEDEF(IMF2DBuffer);//Represents a buffer that contains a two-dimensional surface, such as a video frame. 
_WRL_PTR_TYPEDEF(IMF2DBuffer2);//Represents a buffer that contains a two-dimensional surface, such as a video frame.
_WRL_PTR_TYPEDEF(IMFActivate);//Enables the application to defer the creation of an object.
_WRL_PTR_TYPEDEF(IMFASFContentInfo);//Provides methods to work with the header section of files conforming to the Advanced Systems Format (ASF) specification. 
_WRL_PTR_TYPEDEF(IMFASFIndexer);// Provides methods to work with indexes in Systems Format (ASF) files.
_WRL_PTR_TYPEDEF(IMFASFMultiplexer);//Provides methods to create Advanced Systems Format (ASF) data packets.
_WRL_PTR_TYPEDEF(IMFASFMutualExclusion);//Configures an Advanced Systems Format (ASF) mutual exclusion object, which manages information about a group of streams in an ASF profile that are mutually exclusive.
_WRL_PTR_TYPEDEF(IMFASFProfile);//Manages an Advanced Systems Format (ASF) profile.
_WRL_PTR_TYPEDEF(IMFASFSplitter);//Provides methods to read data from an Advanced Systems Format (ASF) file.
_WRL_PTR_TYPEDEF(IMFASFStreamConfig);//Configures the settings of a stream in an ASF file.
_WRL_PTR_TYPEDEF(IMFASFStreamPrioritization);
_WRL_PTR_TYPEDEF(IMFASFStreamSelector);//Selects streams in an Advanced Systems Format (ASF) file, based on the mutual exclusion information in the ASF header.
_WRL_PTR_TYPEDEF(IMFAsyncCallback);//Callback interface to notify the application when an asynchronous method completes. 
_WRL_PTR_TYPEDEF(IMFAsyncCallbackLogging);//Provides logging information about the parent object the async callback is associated with.
_WRL_PTR_TYPEDEF(IMFAsyncResult);//Provides information about the result of an asynchronous operation. 
_WRL_PTR_TYPEDEF(IMFAttributes);//Provides a generic way to store key/value pairs on an object.
_WRL_PTR_TYPEDEF(IMFAudioMediaType);//IMFAudioMediaType is no longer available for use as of Windows 7.
_WRL_PTR_TYPEDEF(IMFAudioPolicy); // Configures the audio session that is associated with the streaming audio renderer (SAR).
_WRL_PTR_TYPEDEF(IMFAudioStreamVolume); // Controls the volume levels of individual audio channels.
_WRL_PTR_TYPEDEF(IMFByteStream); // Represents a byte stream from some data source, which might be a local file, a network file, or some other source.
_WRL_PTR_TYPEDEF(IMFByteStreamBuffering); // Controls how a byte stream buffers data from a network. 
_WRL_PTR_TYPEDEF(IMFByteStreamCacheControl); // Controls how a network byte stream transfers data to a local cache.
_WRL_PTR_TYPEDEF(IMFByteStreamCacheControl2); // Controls how a network byte stream transfers data to a local cache.
_WRL_PTR_TYPEDEF(IMFByteStreamHandler); // Creates a media source from a byte stream. 
_WRL_PTR_TYPEDEF(IMFByteStreamProxyClassFactory); // Creates a proxy to a byte stream.
_WRL_PTR_TYPEDEF(IMFByteStreamTimeSeek); // Seeks a byte stream by time position.
_WRL_PTR_TYPEDEF(IMFCaptureEngine); // Controls one or more capture device_infos.
_WRL_PTR_TYPEDEF(IMFCaptureEngineClassFactory); // Creates an instance of the capture engine.
_WRL_PTR_TYPEDEF(IMFCaptureEngineOnEventCallback); // Callback interface for receiving events from the capture engine.
_WRL_PTR_TYPEDEF(IMFCaptureEngineOnSampleCallback); // Callback interface to receive data from the capture engine.
_WRL_PTR_TYPEDEF(IMFCapturePhotoSink); // Controls the photo sink.
_WRL_PTR_TYPEDEF(IMFCapturePreviewSink); // Controls the preview sink.
_WRL_PTR_TYPEDEF(IMFCaptureRecordSink); // Controls the recording sink.
_WRL_PTR_TYPEDEF(IMFCaptureSink); // Controls a capture sink, which is an object that receives one or more streams from a capture device.
_WRL_PTR_TYPEDEF(IMFCaptureSource); // Controls the capture source object. The capture source manages the audio and video capture device_infos.
_WRL_PTR_TYPEDEF(IMFClock); // Provides timing information from a clock in Microsoft Media Foundation.
_WRL_PTR_TYPEDEF(IMFClockStateSink); // Receives state-change notifications from the presentation clock. 
_WRL_PTR_TYPEDEF(IMFCollection); // Represents a generic collection of IUnknown pointers. 
_WRL_PTR_TYPEDEF(IMFContentEnabler); // Implements one step that must be performed for the user to access media content.
_WRL_PTR_TYPEDEF(IMFContentProtectionManager); // Enables playback of protected content by providing the application with a pointer to a content enabler object.
_WRL_PTR_TYPEDEF(IMFDesiredSample); // Enables the presenter for the enhanced video renderer (EVR) to request a specific frame from the video mixer.
_WRL_PTR_TYPEDEF(IMFDLNASinkInit); // _WRL_PTR_TYPEDEF(Initializes the Digital Living Network Alliance (DLNA) media sink.); // 
_WRL_PTR_TYPEDEF(IMFDRMNetHelper); // Configures Windows Media Digital Rights Management (DRM) for Network device_infos on a network sink.
_WRL_PTR_TYPEDEF(IMFDXGIBuffer); // Represents a buffer that contains a Microsoft DirectX Graphics Infrastructure (DXGI) surface.
_WRL_PTR_TYPEDEF(IMFDXGIDeviceManager); // Enables two threads to share the same Microsoft Direct3D 11 device.
_WRL_PTR_TYPEDEF(IMFFieldOfUseMFTUnlock); // Enables an application to use a Media Foundation transform (MFT) that has restrictions on its use.
_WRL_PTR_TYPEDEF(IMFFinalizableMediaSink); // Optionally supported by media sinks to perform required tasks before shutdown.
_WRL_PTR_TYPEDEF(IMFGetService); // Queries an object for a specified service interface. 
_WRL_PTR_TYPEDEF(IMFImageSharingEngine); // Enables image sharing.
_WRL_PTR_TYPEDEF(IMFInputTrustAuthority); // Enables other components in the protected media path (PMP) to use the input protection system provided by an input trust authorities (ITA).
_WRL_PTR_TYPEDEF(IMFLocalMFTRegistration); // Registers Media Foundation transforms (MFTs) in the caller's process.
_WRL_PTR_TYPEDEF(IMFMediaBuffer); // Represents a block of memory that contains media data.
_WRL_PTR_TYPEDEF(IMFMediaEngine); // Enables an application to play audio or video files.
_WRL_PTR_TYPEDEF(IMFMediaEngineClassFactory); // Creates an instance of the Media Engine.
_WRL_PTR_TYPEDEF(IMFMediaEngineEx); // Extends the IMFMediaEngine interface.
_WRL_PTR_TYPEDEF(IMFMediaEngineExtension); // Enables an application to load media resources in the Media Engine.
_WRL_PTR_TYPEDEF(IMFMediaEngineNotify); // Callback interface for the IMFMediaEngine interface. 
_WRL_PTR_TYPEDEF(IMFMediaEngineProtectedContent); // Enables the Media Engine to play protected video content.
_WRL_PTR_TYPEDEF(IMFMediaEngineSrcElements); // Provides the Media Engine with a list of media resources.
_WRL_PTR_TYPEDEF(IMFMediaError); // Provides the current error status for the Media Engine.
_WRL_PTR_TYPEDEF(IMFMediaEvent); // Represents an event generated by a Media Foundation object. Use this interface to get information about the event.
_WRL_PTR_TYPEDEF(IMFMediaEventGenerator); // Retrieves events from any Media Foundation object that generates events. 
_WRL_PTR_TYPEDEF(IMFMediaEventQueue); // Provides an event queue for applications that need to implement the IMFMediaEventGenerator interface.
_WRL_PTR_TYPEDEF(IMFMediaSession); // Provides playback controls for protected and unprotected content.
_WRL_PTR_TYPEDEF(IMFMediaSharingEngine); // Enables media sharing.
_WRL_PTR_TYPEDEF(IMFMediaSink); // Implemented by media sink objects.
_WRL_PTR_TYPEDEF(IMFMediaSinkPreroll); // Enables a media sink to receive samples before the presentation clock is started.
_WRL_PTR_TYPEDEF(IMFMediaSource); // Implemented by media source objects.
_WRL_PTR_TYPEDEF(IMFMediaSourceEx); // Extends the IMFMediaSource interface to provide additional capabilities for a media source.
_WRL_PTR_TYPEDEF(IMFMediaSourcePresentationProvider); // Provides notifications to the sequencer source.
_WRL_PTR_TYPEDEF(IMFMediaSourceTopologyProvider); // Enables an application to get a topology from the sequencer source.
_WRL_PTR_TYPEDEF(IMFMediaStream); // Represents one stream in a media source. 
_WRL_PTR_TYPEDEF(IMFMediaTimeRange); // Represents a list of time ranges, where each range is defined by a start and end time.
_WRL_PTR_TYPEDEF(IMFMediaType); // Represents a description of a media format. 
_WRL_PTR_TYPEDEF(IMFMediaTypeHandler); // Gets and sets media types on an object, such as a media source or media sink. 
_WRL_PTR_TYPEDEF(IMFMetadata); // Manages metadata for an object.
_WRL_PTR_TYPEDEF(IMFMetadataProvider); // Gets metadata from a media source or other object.
_WRL_PTR_TYPEDEF(IMFNetCredential); // Sets and retrieves user-name and password information for authentication purposes. 
_WRL_PTR_TYPEDEF(IMFNetCredentialCache); // Gets credentials from the credential cache.
_WRL_PTR_TYPEDEF(IMFNetCredentialManager); // Implemented by applications to provide user credentials for a network source.
_WRL_PTR_TYPEDEF(IMFNetProxyLocator); // Determines the proxy to use when connecting to a server.
_WRL_PTR_TYPEDEF(IMFNetProxyLocatorFactory); // Creates a proxy locator object, which determines the proxy to use.
_WRL_PTR_TYPEDEF(IMFNetResourceFilter); // Notifies the application when a byte stream requests a URL, and enables the application to block URL redirection.
_WRL_PTR_TYPEDEF(IMFNetSchemeHandlerConfig); // Configures a network scheme plug-in. 
_WRL_PTR_TYPEDEF(IMFObjectReferenceStream); // Marshals an interface pointer to and from a stream. Stream objects that support IStream can expose this interface to provide custom marshaling for interface pointers.
_WRL_PTR_TYPEDEF(IMFOutputPolicy); // Encapsulates a usage policy from an input trust authority (ITA).
_WRL_PTR_TYPEDEF(IMFOutputSchema); // Encapsulates information about an output protection system and its corresponding configuration data.
_WRL_PTR_TYPEDEF(IMFOutputTrustAuthority); // Encapsulates the functionality of one or more output protection systems that a trusted output supports.
_WRL_PTR_TYPEDEF(IMFPluginControl); // Controls how media sources and transforms are enumerated in Media Foundation.
_WRL_PTR_TYPEDEF(IMFPluginControl2); // Controls how media sources and transforms are enumerated in Media Foundation.
_WRL_PTR_TYPEDEF(IMFPMPClient); // Enables a media source to receive a pointer to the IMFPMPHost interface. 
_WRL_PTR_TYPEDEF(IMFPMPClientApp); // Provides a mechanism for a media source to implement content protection functionality in a Windows Store apps.
_WRL_PTR_TYPEDEF(IMFPMPHost); // Enables a media source in the application process to create objects in the protected media path (PMP) process.
_WRL_PTR_TYPEDEF(IMFPMPHostApp); // Allows a media source to create a Windows Runtime object in the Protected Media Path (PMP) process. 
_WRL_PTR_TYPEDEF(IMFPMPServer); // Enables two instances of the Media Session to share the same protected media path (PMP) process. 
_WRL_PTR_TYPEDEF(IMFPresentationClock); // Represents a presentation clock, which is used to schedule when samples are rendered and to synchronize multiple streams.
_WRL_PTR_TYPEDEF(IMFPresentationDescriptor); // Describes the details of a presentation. A presentation is a set of related media streams that share a common presentation time. 
_WRL_PTR_TYPEDEF(IMFPresentationTimeSource); // Provides the clock times for the presentation clock. 
_WRL_PTR_TYPEDEF(IMFProtectedEnvironmentAccess); // Provides a method that allows content protection systems to perform a handshake with the protected environment. This is needed because the CreateFile and DeviceIoControl APIs are not available to Windows Store apps.
_WRL_PTR_TYPEDEF(IMFQualityAdvise); // Enables the quality manager to adjust the audio or video quality of a component in the pipeline.
_WRL_PTR_TYPEDEF(IMFQualityAdvise2); // Enables a pipeline object to adjust its own audio or video quality, in response to quality messages.
_WRL_PTR_TYPEDEF(IMFQualityAdviseLimits); // Queries an object for the number of quality modes it supports.
_WRL_PTR_TYPEDEF(IMFQualityManager); // Adjusts playback quality. This interface is exposed by the quality manager. 
_WRL_PTR_TYPEDEF(IMFRateControl); // Gets or sets the playback rate. 
_WRL_PTR_TYPEDEF(IMFRateSupport); // Queries the range of playback rates that are supported, including reverse playback.
_WRL_PTR_TYPEDEF(IMFReadWriteClassFactory); // Creates an instance of either the sink writer or the source reader.
_WRL_PTR_TYPEDEF(IMFRealTimeClient); // Notifies a pipeline object to register itself with the Multimedia Class Scheduler Service (MMCSS).
_WRL_PTR_TYPEDEF(IMFRealTimeClientEx); // Notifies a pipeline object to register itself with the Multimedia Class Scheduler Service (MMCSS). 
_WRL_PTR_TYPEDEF(IMFRemoteAsyncCallback); // Used by the Media Foundation proxy/stub DLL to marshal certain asynchronous method calls across process boundaries.Applications do not use or implement this interface.
_WRL_PTR_TYPEDEF(IMFRemoteDesktopPlugin); // Modifies a topology for use in a Terminal Services environment. 
_WRL_PTR_TYPEDEF(IMFRemoteProxy); // Exposed by objects that act as a proxy for a remote object.
_WRL_PTR_TYPEDEF(IMFSAMIStyle); // Sets and retrieves Synchronized Accessible Media Interchange (SAMI) styles on the SAMI Media Source. 
_WRL_PTR_TYPEDEF(IMFSample); // Represents a media sample, which is a container object for media data.
_WRL_PTR_TYPEDEF(IMFSampleGrabberSinkCallback); // Callback interface to get media data from the sample-grabber sink. 
_WRL_PTR_TYPEDEF(IMFSampleGrabberSinkCallback2); // Extends the IMFSampleGrabberSinkCallback interface.
_WRL_PTR_TYPEDEF(IMFSampleOutputStream); // Writes media samples to a byte stream.
_WRL_PTR_TYPEDEF(IMFSampleProtection); // Provides encryption for media data inside the protected media path (PMP). 
_WRL_PTR_TYPEDEF(IMFSaveJob); // Persists media data from a source byte stream to an application-provided byte stream.
_WRL_PTR_TYPEDEF(IMFSchemeHandler); // Creates a media source or a byte stream from a URL. 
_WRL_PTR_TYPEDEF(IMFSecureChannel); // Establishes a one-way secure channel between two objects. 
_WRL_PTR_TYPEDEF(IMFSeekInfo); // For a particular seek position, gets the two nearest key frames.
_WRL_PTR_TYPEDEF(IMFSequencerSource); // Implemented by the Sequencer Source.
_WRL_PTR_TYPEDEF(IMFSharingEngineClassFactory); // Creates an instance of the media sharing engine.
_WRL_PTR_TYPEDEF(IMFShutdown); // Exposed by some Media Foundation objects that must be explicitly shut down. 
_WRL_PTR_TYPEDEF(IMFSignedLibrary); // Provides a method that allows content protection systems to get the procedure address of a function in the signed library. This method provides the same functionality as GetProcAddress which is not available to Windows Store apps.
_WRL_PTR_TYPEDEF(IMFSimpleAudioVolume); // Controls the master volume level of the audio session associated with the streaming audio renderer (SAR) and the audio capture source.
_WRL_PTR_TYPEDEF(IMFSinkWriter); // Implemented by the Media Foundation sink writer object.
_WRL_PTR_TYPEDEF(IMFSinkWriterCallback); // Callback interface for the Media Foundation sink writer. 
_WRL_PTR_TYPEDEF(IMFSinkWriterEx); // Extends the IMFSinkWriter interface.
_WRL_PTR_TYPEDEF(IMFSourceOpenMonitor); // Callback interface to receive notifications from a network source on the progress of an asynchronous open operation.
_WRL_PTR_TYPEDEF(IMFSourceReader); // Implemented by the Media Foundation source reader object.
_WRL_PTR_TYPEDEF(IMFSourceReaderCallback); // Callback interface for the Media Foundation source reader.
_WRL_PTR_TYPEDEF(IMFSourceReaderEx); // Extends the IMFSourceReader interface.
_WRL_PTR_TYPEDEF(IMFSourceResolver); // Creates a media source from a URL or a byte stream.
_WRL_PTR_TYPEDEF(IMFSSLCertificateManager); // _WRL_PTR_TYPEDEF(Implemented by a client and called by Media Foundation to get the client Secure Sockets Layer (SSL) certificate requested by the server.); // 
_WRL_PTR_TYPEDEF(IMFStreamDescriptor); // Gets information about one stream in a media source. 
_WRL_PTR_TYPEDEF(IMFStreamingSinkConfig); // Passes configuration information to the media sinks that are used for streaming the content.
_WRL_PTR_TYPEDEF(IMFStreamSink); // Represents a stream on a media sink object.
_WRL_PTR_TYPEDEF(IMFSystemId); // Provides a method that retireves system id data.
_WRL_PTR_TYPEDEF(IMFTimecodeTranslate); // Converts between Society of Motion Picture and Television Engineers (SMPTE) time codes and 100-nanosecond time units.
_WRL_PTR_TYPEDEF(IMFTimer); // Provides a timer that invokes a callback at a specified time.
_WRL_PTR_TYPEDEF(IMFTopoLoader); // Converts a partial topology into a full topology.
_WRL_PTR_TYPEDEF(IMFTopology); // Represents a topology. A topology describes a collection of media sources, sinks, and transforms that are connected in a certain order.
_WRL_PTR_TYPEDEF(IMFTopologyNode); // Represents a node in a topology.
_WRL_PTR_TYPEDEF(IMFTopologyNodeAttributeEditor); // Updates the attributes of one or more nodes in the Media Session's current topology.
_WRL_PTR_TYPEDEF(IMFTopologyServiceLookup); // Enables a custom video mixer or video presenter to get interface pointers from the Enhanced Video Renderer (EVR).
_WRL_PTR_TYPEDEF(IMFTopologyServiceLookupClient); // Initializes a video mixer or presenter.
_WRL_PTR_TYPEDEF(IMFTrackedSample); // Tracks the reference counts on a video media sample.
_WRL_PTR_TYPEDEF(IMFTranscodeProfile); // Implemented by the transcode profile object.
_WRL_PTR_TYPEDEF(IMFTranscodeSinkInfoProvider); // Implemented by the transcode sink activation object.
_WRL_PTR_TYPEDEF(IMFTransform); // Implemented by all Media Foundation Transforms (MFTs).
_WRL_PTR_TYPEDEF(IMFTrustedInput); // _WRL_PTR_TYPEDEF(Implemented by components that provide input trust authorities (ITAs). This interface is used to get the ITA for each of the component's streams.); // 
_WRL_PTR_TYPEDEF(IMFTrustedOutput); // Implemented by components that provide output trust authorities (OTAs).
_WRL_PTR_TYPEDEF(IMFVideoDeviceID); // Returns the device identifier supported by a video renderer component.
_WRL_PTR_TYPEDEF(IMFVideoDisplayControl); // Controls how the Enhanced Video Renderer (EVR) displays video.
_WRL_PTR_TYPEDEF(IMFVideoMediaType); // Represents a description of a video format.
//_WRL_PTR_TYPEDEF(IMFVideoMixerBitmap); // Alpha-blends a static bitmap image with the video displayed by the Enhanced Video Renderer (EVR).
_WRL_PTR_TYPEDEF(IMFVideoMixerControl); // Controls how the Enhanced Video Renderer (EVR) mixes video substreams.
_WRL_PTR_TYPEDEF(IMFVideoMixerControl2); // Controls preferences for video deinterlacing.
_WRL_PTR_TYPEDEF(IMFVideoPositionMapper); // Maps a position on an input video stream to the corresponding position on an output video stream.
_WRL_PTR_TYPEDEF(IMFVideoPresenter); // Represents a video presenter. A video presenter is an object that receives video frames, typically from a video mixer, and presents them in some way, typically by rendering them to the display.
//_WRL_PTR_TYPEDEF(IMFVideoProcessor); // Controls video processing in the Enhanced Video Renderer (EVR).
_WRL_PTR_TYPEDEF(IMFVideoProcessorControl); // Configures the Video Processor MFT.
_WRL_PTR_TYPEDEF(IMFVideoRenderer); // Sets a new mixer or presenter for the Enhanced Video Renderer (EVR).
_WRL_PTR_TYPEDEF(IMFVideoSampleAllocator); // Allocates video samples for a video media sink.
_WRL_PTR_TYPEDEF(IMFVideoSampleAllocatorCallback); // Enables an application to track video samples allocated by the enhanced video renderer (EVR).
_WRL_PTR_TYPEDEF(IMFVideoSampleAllocatorEx); // Allocates video samples that contain Direct3D 11 texture surfaces.
_WRL_PTR_TYPEDEF(IMFVideoSampleAllocatorNotify); // The callback for the IMFVideoSampleAllocatorCallback interface.
_WRL_PTR_TYPEDEF(IMFWorkQueueServices); // Controls the work queues created by the Media Session.
_WRL_PTR_TYPEDEF(IMFWorkQueueServicesEx); // Extends the IMFWorkQueueServices interface.
_WRL_PTR_TYPEDEF(IPlayToControl); // Enables the PlayToConnection object to connect to a media element.
_WRL_PTR_TYPEDEF(IPlayToSourceClassFactory); // Creates an instance of the PlayToSource object.
_WRL_PTR_TYPEDEF(IWMCodecLeakyBucket); // Configures the "leaky bucket" parameters on a video encoder.
_WRL_PTR_TYPEDEF(IWMCodecOutputTimestamp); // Gets the time stamp of the next video frame to be decoded.
_WRL_PTR_TYPEDEF(IWMCodecPrivateData); // Gets the private codec data that must be appended to the output media type. This codec data is required for properly decoding Windows Media Video content.
_WRL_PTR_TYPEDEF(IWMCodecProps); // Provides methods that retrieve format-specific codec properties.
_WRL_PTR_TYPEDEF(IWMCodecStrings); // Retrieves names and descriptive strings for codecs and formats.
_WRL_PTR_TYPEDEF(IWMColorConvProps); // Sets properties on the color converter DSP. 
_WRL_PTR_TYPEDEF(IWMResamplerProps); // Sets properties on the audio resampler DSP. 
_WRL_PTR_TYPEDEF(IWMResizerProps); // Sets properties on the video resizer DSP.
_WRL_PTR_TYPEDEF(IWMSampleExtensionSupport); // Configures codec support for sample extensions. 
_WRL_PTR_TYPEDEF(IWMVideoDecoderHurryup); // Controls the speed of the video decoder.
_WRL_PTR_TYPEDEF(IWMVideoForceKeyFrame); // Forces the encoder to encode the current frame as a key frame. 

namespace sf {
  // プロパティ用バリアントのラッパ
  struct prop_variant 
  {
    prop_variant()
    {
      PropVariantInit(&value_);// 
    }

    ~prop_variant()
    {
      PropVariantClear(&value_);
    }

    PROPVARIANT* get(){ return &value_;};

    PROPVARIANT* operator &(){return get();}

    operator PROPVARIANT*() {return get();}

  private:
    PROPVARIANT value_;
  };

  template <class Q>
  HRESULT GetEventObject(IMFMediaEvent *pEvent, Q **ppObject)
  {
    *ppObject = NULL;   // zero output

    PROPVARIANT var;
    HRESULT hr = pEvent->GetValue(&var);
    if (SUCCEEDED(hr))
    {
      if (var.vt == VT_UNKNOWN)
      {
        hr = var.punkVal->QueryInterface(ppObject);
      }
      else
      {
        hr = MF_E_INVALIDTYPE;
      }
      PropVariantClear(&var);
    }
    return hr;
  }

#define VIDEO_WIDTH  1280
#define VIDEO_HEIGHT  720

  extern HRESULT InitializeSinkWriter(IMFSinkWriter **ppWriter, DWORD *pStreamIndex);
  extern HRESULT WriteFrame(
    IMFSinkWriter *pWriter, 
    DWORD streamIndex, 
    const LONGLONG& rtStart,        // Time stamp.
    const LONGLONG& rtDuration      // Frame duration.
    );
  extern Windows::Foundation::IAsyncActionWithProgress<double>^ WriteAsync(Windows::Storage::Streams::IRandomAccessStream^ stream);

  // Class to start and shutdown Media Foundation
  class AutoMF
  {
  public:
    AutoMF()
      : _bInitialized(false)
    {
      CHK(MFStartup(MF_VERSION));
    }

    ~AutoMF()
    {
      if (_bInitialized)
      {
        (void) MFShutdown();
      }
    }

  private:
    bool _bInitialized;
  };
  ref class  VideoWriter sealed
  {
  public:
    VideoWriter(
      Windows::Storage::Streams::IRandomAccessStream^ stream
      );

    virtual ~VideoWriter()
    {
      Finalize();
    };
  internal:
    virtual void Finalize()
    {
      if(sinkWriter_)
      {
        sinkWriter_->Finalize();
      }
    }
    void WriteTexture(ID3D11DeviceContext1Ptr& context ,ID3D11Texture2DPtr& texture);
  private:
    Windows::Storage::Streams::IRandomAccessStream^ stream_;
    IMFSinkWriterPtr sinkWriter_;
    IMFByteStreamPtr byteStream_;
    IMFAttributesPtr attr_;
    IMFMediaTypePtr mediaTypeOut_;
    IMFMediaTypePtr mediaTypeIn_;
    DWORD streamIndex_;
    IMFSamplePtr sample_;
    IMFMediaBufferPtr buffer_;
    LONGLONG sampleTime_;
  };
}
