#include "SKPRColorBlob.h"
#include "SKPRPureImage.h"
#include "KFRDisplay.h"
#include "SKConfig.h"
#include "SKPRBodyFrameTracker.h"
#include "SKPRSplitter.h"
#include "SKPRPersonTracker.h"
#include "SKWrapper.h"
#include "ros/ros.h"
#include "std_msgs/String.h"


#include <sstream>
#include <gtk/gtk.h>
#include <iostream>

bool keepRunning = true;
GtkWidget *window;

void *skwThread(void *data) {
    DoOnce *skw = (DoOnce *)data;
    while(keepRunning) {
        ROS_INFO_STREAM("A");
        skw->doOnce();
        ros::spinOnce();
        //loop_rate.sleep();
        ROS_INFO_STREAM("/A");
    }
    return NULL;
}


gboolean exit_program(GtkWidget *widget, GdkEvent *event, gpointer data) {
    //if menu closed exit program entirely. 
    keepRunning = false;
    //exit(0);
    return TRUE;
}


static void buildUI (GtkApplication *app, gpointer user_data){
    ROS_INFO_STREAM("B");
    window = gtk_application_window_new(app);
    gtk_window_set_title (GTK_WINDOW (window), "Single Kinect" );
    gtk_window_set_default_size(GTK_WINDOW(window), 1920, 1080);

    gtk_widget_add_events(window, GDK_KEY_PRESS_MASK);

    KFRDisplay* kfrd = (KFRDisplay*) user_data;

    kfrd->buildWidgets(window);

    g_signal_connect(window, "destroy", G_CALLBACK(exit_program), NULL);
    gtk_widget_show_all (window);
    ROS_INFO_STREAM("/B");
}

using namespace std;

int main(int argc, char **argv) {
  g_thread_init(NULL);
  ros::init(argc, argv, "node_camera");

  ros::NodeHandle n;

  SKConfig skc;
  SKWrapper skw(skc);


  SKPRPureImage img;
  SKPRColorBlob blob(n);
  KFRDisplay kfrDisplay("blue blobs");

  skw.setRecipient(&img);
  img.setRecipient(&blob);
  blob.setRecipient(&kfrDisplay);


  GtkApplication *app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (buildUI), &kfrDisplay);

    pthread_t thread;
    pthread_create(&thread, NULL, skwThread, &skw);

  int status = g_application_run (G_APPLICATION (app), 0, argv);


  g_object_unref (app);
  pthread_join(thread, NULL);

  return 0;
}