#include <gst/gst.h>

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

int tutorial_main(int argc, char *argv[])
{
	GstElement *pipeline;
	GstBus *bus;
	GstMessage *msg;

	/*Intialize GStreamer */
	gst_init(&argc, &argv);

	pipeline = gst_parse_launch("playbin uri=https://www.freedesktop.org/software/gstreamer-sdk/data/media/sintel_trailer-480p.webm",NULL);

	//playing
	gst_element_set_state(pipeline, GST_STATE_PLAYING);

	/* wait until error or end of signal */
	bus  =  gst_element_get_bus(pipeline);
	msg = gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE,GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

	if(GST_MESSAGE_TYPE(msg) == GST_MESSAGE_ERROR) {
		g_error("An error occurred! Re-run with the GST_DEBUG");
	}

	//free resource
	gst_message_unref (msg);
	gst_object_unref (bus);
	gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_object_unref(pipeline);
}

int main(int argc, char* argv[])
{
	#if defined(__APPLE__) && TARGET_OS_MAC && !TARGET_OS_IPHONE
  		return gst_macos_main (tutorial_main, argc, argv, NULL);
	#else
  	return tutorial_main (argc, argv);
	#endif
}

