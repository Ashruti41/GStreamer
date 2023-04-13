/* gst-launch-1.0 -v filesrc location=test.mp4 ! qtdemux ! h264parse ! omxh264dec ! x264enc ! rtph264pay ! udpsink host=$HOST port=5000 */


#include <gst/gst.h>

/*structure to contain all our information , so we can pass it to call backs */
typedef struct _CustomData {
	GstElement *pipeline;
	GstElement *filesrc;
	GstPad *fileblock;
	GstElement *qtdemux; //demultiplexer
	GstElement *parse;
	GstElement *decoder;
	GstElement *enc;
	GstElement *rtp;
	GstElement *udp;
} CustomData;

int main(int argc, char *argv[])
{
	CustomData data;
	GstBus *bus;
	GstMessage *msg;
	GstStateChangeReturn ret;
	gboolean terminate = FALSE;

	/*init gstreamer */
	gst_init(&argc, &argv);

	/*create elements */
	data.pipeline = gst_pipeline_new("pipeline");

	data.filesrc = gst_element_factory_make("filesrc",NULL);
	if(!data.filesrc) {
		GST_ERROR("Failed to create filesrc");
		return -1;
	}
	g_object_set(G_OBJECT(data.filesrc), "location","/home/ashruti/sample.mp4", NULL);
	
	data.fileblock = gst_element_get_static_pad(data.filesrc, "src");

	data.qtdemux = gst_element_factory_make("qtdemux", NULL);
	if(!data.qtdemux) {
		GST_ERROR("faied to create qtdemux");
		return -1;
	}

	data.parse = gst_element_factory_make("h264parse", NULL);
	if(!data.parse) {
		GST_ERROR("failed to create parse");
		return -1;
	}

	data.decoder = gst_element_factory_make("avdec_h264",NULL);
	if(!data.decoder) {
		GST_ERROR("failed to create decoder");
		return -1;
	}

	data.enc = gst_element_factory_make("x264enc", NULL);
	if(!data.enc) {
		GST_ERROR("failed to create encoder");
		return -1;
	}

	data.rtp = gst_element_factory_make("rtph264pay", NULL);
	if(!data.rtp) {
		GST_ERROR("failed to create rtph264pay.");
		return -1;
	}

	data.udp = gst_element_factory_make("udpsink", NULL);
	if(!data.udp) {
		GST_ERROR("failed to create udpsink");
		return -1;
	}

	g_object_set(G_OBJECT(data.udp), "host", "127.0.0.1", NULL);
	g_object_set(G_OBJECT(data.udp), "port", 5000, NULL);

	if(!data.pipeline || !data.filesrc || !data.qtdemux || !data.parse || !data.decoder || !data.enc || !data.rtp || !data.udp ) {
		GST_ERROR("unable to create all elements!");
		return -1;
	}

	gst_bin_add_many(GST_BIN(data.pipeline), data.filesrc, data.qtdemux, data.parse, data.decoder, data.enc, data.rtp, data.udp,NULL);

	/* Links all elements together */
	gst_element_link(data.filesrc , data.qtdemux);
	gst_element_link(data.qtdemux, data.parse);
	gst_element_link(data.parse, data.decoder);
	gst_element_link(data.decoder, data.enc);
	gst_element_link(data.enc, data.rtp);
	gst_element_link(data.rtp, data.udp);

	ret = gst_element_set_state(data.pipeline, GST_STATE_PLAYING);
	if(ret == GST_STATE_CHANGE_FAILURE) {
		GST_ERROR("unable to set the pipeline to the playing state.\n");
		gst_object_unref(data.pipeline);
		return -1;
	}

	/* Listen to the bus */
	bus = gst_element_get_bus(data.pipeline);
	do {
    		msg = gst_bus_timed_pop_filtered (bus, GST_CLOCK_TIME_NONE,
        	GST_MESSAGE_STATE_CHANGED | GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

    		/* Parse message */
    		if (msg != NULL) {
     	 	GError *err;
      		gchar *debug_info;

      		switch (GST_MESSAGE_TYPE (msg)) {
        		case GST_MESSAGE_ERROR:
          			gst_message_parse_error (msg, &err, &debug_info);
          			g_printerr ("Error received from element %s: %s\n", GST_OBJECT_NAME (msg->src), err->message);
          			g_printerr ("Debugging information: %s\n", debug_info ? debug_info : "none");
          			g_clear_error (&err);
          			g_free (debug_info);
          			terminate = TRUE;
          		break;
        		case GST_MESSAGE_EOS:
          			g_print ("End-Of-Stream reached.\n");
          			terminate = TRUE;
          		break;
        		case GST_MESSAGE_STATE_CHANGED:
         	 /* We are only interested in state-changed messages from the pipeline */
          			if (GST_MESSAGE_SRC (msg) == GST_OBJECT (data.pipeline)) {
            				GstState old_state, new_state, pending_state;
            				gst_message_parse_state_changed (msg, &old_state, &new_state, &pending_state);
            				g_print ("Pipeline state changed from %s to %s:\n",
                			gst_element_state_get_name (old_state), gst_element_state_get_name (new_state));
          			}
          		break;
        		default:
          		/* We should not reach here */
          		g_printerr ("Unexpected message received.\n");
          		break;
      		}
		gst_object_unref(msg);
		}
	}while(!terminate);

	/*free resources */
	gst_object_unref(bus);
	gst_element_set_state(data.pipeline, GST_STATE_NULL);
	gst_object_unref(data.pipeline);
	return 0;
}


	

