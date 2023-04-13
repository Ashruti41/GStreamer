/* gst-launch-1.0 filesrc location=/Users/siyao/Movies/why_I_left_China_for_Good.mp4 ! qtdemux name=demux  demux.audio_0 ! queue ! decodebin ! audioconvert ! audioresample ! autoaudiosink  demux.video_0 ! queue ! decodebin ! videoconvert ! videoscale ! autovideosink */


#include <gst/gst.h>

int main(int argc, char *argv[])
{
	GstElement *pipeline, *source, *demux, *queue, *decode, *audioconvert, *resample, *audiosink, *convert, *scale, *sink;
	GstBus *bus;
	GstMessage *msg;
	GstStateChangeReturn ret;

	gst_init(&argc, &argv);

	//create pipeline
	pipeline = gst_pipeline_new("pipeline");

	//create elements
	source = gst_element_factory_make("filesrc", NULL);
	g_object_set(G_OBJECT(source), "location", "/home/ashruti/test.mp4", NULL);

	demux = gst_element_factory_make("qtdemux", "demux");
	g_object_set(G_OBJECT(demux),0,"demux.audio_0",NULL);

	queue = gst_element_factory_make("queue", NULL);
	decode = gst_element_factory_make("decodebin", NULL);
	audioconvert = gst_element_factory_make("audioconvert", NULL);
	resample = gst_element_factory_make("audioresample", NULL);
	
	audiosink = gst_element_factory_make("autoaudiosink", NULL);
	g_object_set(G_OBJECT(audiosink),0, "demux.video_0", NULL);

	convert = gst_element_factory_make("videoconvert", NULL);
	scale = gst_element_factory_make("videoscale", NULL);
	sink = gst_element_factory_make("autovideosink",NULL);

	if(!pipeline || !source || !demux || !queue || !decode || !audioconvert || !resample || !audiosink || !convert || !scale || !sink )
	{
		g_printerr("failed to create elements");
		return -1;
	}

	gst_bin_add_many(GST_BIN(pipeline),source, demux, queue, decode, audioconvert, resample, audiosink, convert, scale, sink, NULL);

	//link elements
	gst_element_link(source, demux);
	gst_element_link(demux, queue);
	gst_element_link(queue, decode);
	gst_element_link(decode, audioconvert);
	gst_element_link(audioconvert, resample);
	gst_element_link(resample, audiosink);
	gst_element_link(audiosink, queue);
	gst_element_link(queue, decode);
	gst_element_link(decode, convert);
	gst_element_link(convert, scale);
	gst_element_link(scale, sink);

	 /* Start playing */
  	ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
  	if (ret == GST_STATE_CHANGE_FAILURE) {
    		g_printerr ("Unable to set the pipeline to the playing state.\n");
    		gst_object_unref (pipeline);
    		return -1;
  	}

  	/* Wait until error or EOS */
  	bus = gst_element_get_bus (pipeline);
  	msg = gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

  	/* Parse message */
  	if (msg != NULL) {
    		GError *err;
    		gchar *debug_info;

    	switch (GST_MESSAGE_TYPE (msg)) {
      	case GST_MESSAGE_ERROR:
        	gst_message_parse_error (msg, &err, &debug_info);
        	g_printerr ("Error received from element %s: %s\n",
            	GST_OBJECT_NAME (msg->src), err->message);
        	g_printerr ("Debugging information: %s\n",
            	debug_info ? debug_info : "none");
        	g_clear_error (&err);
        	g_free (debug_info);
       	break;
      	case GST_MESSAGE_EOS:
        	g_print ("End-Of-Stream reached.\n");
      	break;
      	default:
        /* We should not reach here because we only asked for ERRORs and EOS */
        	g_printerr ("Unexpected message received.\n");
        break;
    	}
    	gst_message_unref (msg);
  	}

	gst_object_unref(bus);
	gst_element_set_state(pipeline, GST_STATE_NULL);
	gst_object_unref(pipeline);
	return 0;
}


	

