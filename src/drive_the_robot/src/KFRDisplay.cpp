#include "KFRDisplay.h"
#include "SKConfig.h"
#include "SKPacket.h"
//#include "conversions.h"

#include <iostream>
using namespace std;

KFRDisplay::KFRDisplay(std::string imgName) :
    _initialized(false),
    _rows(1080), _cols(1920),
    _bytes(_cols * _rows * 3),
    _drawingArea(NULL),
    _buf((unsigned char *)calloc(1, _bytes)),
    _imgName(imgName)
    {}

KFRDisplay::~KFRDisplay() {}

void KFRDisplay::buildWidgets(GtkWidget *container) {
    _drawingArea = gtk_drawing_area_new();

    gtk_container_add(GTK_CONTAINER (container), (GtkWidget * ) _drawingArea);

    g_signal_connect (G_OBJECT (_drawingArea), "draw", G_CALLBACK (drawCallback), this);

    _initialized = true;

}

void KFRDisplay::receiveFrame(SKPacket &skp) {
    //memcpy(_buf, skp.getRGBColorPreviewScale().data, _bytes);
    memcpy(_buf, skp.getCVMat(_imgName).data, _bytes);
    
    //Draw each area
    if(_initialized) {
        gtk_widget_queue_draw(_drawingArea);
    }
}

gboolean KFRDisplay::drawCallback(GtkWidget *widget, cairo_t *cr, gpointer data) {
    KFRDisplay *display = (KFRDisplay *)data;    
    return display->doDraw(cr);
}

gboolean KFRDisplay::doDraw(cairo_t *cr) {
    if(_buf != NULL) {
        GdkPixbuf *pixbuf = gdk_pixbuf_new_from_data(
            (guint8*)(_buf),
            GDK_COLORSPACE_RGB,
            false,
            8,
            _cols,
            _rows,
            (int)3 * _cols, NULL, NULL);
        gdk_cairo_set_source_pixbuf(cr, pixbuf, 0, 0);
        cairo_paint(cr);
        gdk_pixbuf_unref(pixbuf);
    }
    return TRUE;
}