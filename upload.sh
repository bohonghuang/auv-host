#!/bin/bash
gst-launch-1.0 v4l2src device=/dev/video0 ! video/x-raw, format=YUY2,width=640,height=360 ! videoconvert ! nvvidconv ! nvv4l2h264enc ! rtph264pay ! udpsink host=192.168.31.100 port=5600