/* GTK - The GIMP Toolkit
 * Copyright (C) 1995-1997 Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/*
 * GtkTicker Copyright 2000 Syd Logan
 */

#include "gtkticker.h"

static void gtk_ticker_compute_offsets (GtkTicker    *ticker);
static void gtk_ticker_class_init    (GtkTickerClass    *klass);
static void gtk_ticker_init          (GtkTicker         *ticker);
static void gtk_ticker_map           (GtkWidget        *widget);
static void gtk_ticker_realize       (GtkWidget        *widget);
static void gtk_ticker_size_request  (GtkWidget        *widget,
				     GtkRequisition   *requisition);
static void gtk_ticker_size_allocate (GtkWidget        *widget,
				     GtkAllocation    *allocation);
static void gtk_ticker_paint         (GtkWidget        *widget,
				     GdkRectangle     *area);
static void gtk_ticker_draw          (GtkWidget        *widget,
				     GdkRectangle     *area);
static gint gtk_ticker_expose        (GtkWidget        *widget,
				     GdkEventExpose   *event);
static void gtk_ticker_add_real      (GtkContainer     *container,
				     GtkWidget        *widget);
static void gtk_ticker_remove_real   (GtkContainer     *container,
				     GtkWidget        *widget);
static void gtk_ticker_forall        (GtkContainer     *container,
				     gboolean 	       include_internals,
				     GtkCallback       callback,
				     gpointer          callback_data);
static GtkType gtk_ticker_child_type (GtkContainer     *container);


static GtkContainerClass *parent_class = NULL;


GtkType
gtk_ticker_get_type (void)
{
  static GtkType ticker_type = 0;

  if (!ticker_type)
    {
      static const GtkTypeInfo ticker_info =
      {
	"GtkTicker",
	sizeof (GtkTicker),
	sizeof (GtkTickerClass),
	(GtkClassInitFunc) gtk_ticker_class_init,
	(GtkObjectInitFunc) gtk_ticker_init,
	/* reserved_1 */ NULL,
        /* reserved_2 */ NULL,
        (GtkClassInitFunc) NULL,
      };

      ticker_type = gtk_type_unique (GTK_TYPE_CONTAINER, &ticker_info);
    }

  return ticker_type;
}

static void
gtk_ticker_class_init (GtkTickerClass *class)
{
  GtkObjectClass *object_class;
  GtkWidgetClass *widget_class;
  GtkContainerClass *container_class;

  object_class = (GtkObjectClass*) class;
  widget_class = (GtkWidgetClass*) class;
  container_class = (GtkContainerClass*) class;

  parent_class = gtk_type_class (GTK_TYPE_CONTAINER);

  widget_class->map = gtk_ticker_map;
  widget_class->realize = gtk_ticker_realize;
  widget_class->size_request = gtk_ticker_size_request;
  widget_class->size_allocate = gtk_ticker_size_allocate;
  widget_class->draw = gtk_ticker_draw;
  widget_class->expose_event = gtk_ticker_expose;

  container_class->add = gtk_ticker_add_real;
  container_class->remove = gtk_ticker_remove_real;
  container_class->forall = gtk_ticker_forall;
  container_class->child_type = gtk_ticker_child_type;
}

static GtkType
gtk_ticker_child_type (GtkContainer *container)
{
  return GTK_TYPE_WIDGET;
}

static void
gtk_ticker_init (GtkTicker *ticker)
{
  GTK_WIDGET_UNSET_FLAGS (ticker, GTK_NO_WINDOW);

  ticker->interval = (guint) 200; 
  ticker->scootch = (guint) 2; 
  ticker->children = NULL;
  ticker->timer = 0;
  ticker->dirty = TRUE;
}

GtkWidget*
gtk_ticker_new (void)
{
  GtkTicker *ticker;

  ticker = gtk_type_new (GTK_TYPE_TICKER);
  return GTK_WIDGET (ticker);
}

static void
gtk_ticker_put (GtkTicker       *ticker,
               GtkWidget      *widget)
{
  GtkTickerChild *child_info;

  g_return_if_fail (ticker != NULL);
  g_return_if_fail (GTK_IS_TICKER (ticker));
  g_return_if_fail (widget != NULL);

  child_info = g_new(GtkTickerChild, 1);
  child_info->widget = widget;
  child_info->x = 0;

  gtk_widget_set_parent(widget, GTK_WIDGET (ticker));

  ticker->children = g_list_append (ticker->children, child_info); 

  if (GTK_WIDGET_REALIZED (ticker))
    gtk_widget_realize (widget);

  if (GTK_WIDGET_VISIBLE (ticker) && GTK_WIDGET_VISIBLE (widget))
    {
      if (GTK_WIDGET_MAPPED (ticker))
	gtk_widget_map (widget);
      
      gtk_widget_queue_resize (GTK_WIDGET (ticker));
    }
}

void
gtk_ticker_set_interval (GtkTicker *ticker, gint interval )
{
  g_return_if_fail (ticker != NULL);
  g_return_if_fail (GTK_IS_TICKER (ticker));

  if ( interval < 0 )
	interval = 200;
  ticker->interval = interval; 
	
}

guint
gtk_ticker_get_interval (GtkTicker *ticker )
{
  g_return_val_if_fail (ticker != NULL, -1);
  g_return_val_if_fail (GTK_IS_TICKER (ticker), -1);

  return ticker->interval;
}

void
gtk_ticker_set_scootch (GtkTicker *ticker, gint scootch )
{
  g_return_if_fail (ticker != NULL);
  g_return_if_fail (GTK_IS_TICKER (ticker));

  if ( scootch <= 0 )
	scootch = 2;
  ticker->scootch = scootch; 
  ticker->dirty = TRUE;	
}

guint
gtk_ticker_get_scootch (GtkTicker *ticker )
{
  g_return_val_if_fail (ticker != NULL, -1);
  g_return_val_if_fail (GTK_IS_TICKER (ticker), -1);

  return ticker->scootch;
}

void
gtk_ticker_set_spacing (GtkTicker *ticker, gint spacing )
{
  g_return_if_fail (ticker != NULL);
  g_return_if_fail (GTK_IS_TICKER (ticker));

  if ( spacing < 0 )
	spacing = 0;
  ticker->spacing = spacing; 
  ticker->dirty = TRUE;	
	
}

static int
ticker_timeout( gpointer data )
{
	GtkTicker *ticker = (GtkTicker *) data;

      	if (GTK_WIDGET_VISIBLE (ticker))
     		gtk_widget_queue_resize (GTK_WIDGET (ticker));

	return( TRUE );
}

void       
gtk_ticker_start_scroll(GtkTicker *ticker)
{
  	g_return_if_fail (ticker != NULL);
  	g_return_if_fail (GTK_IS_TICKER (ticker));
	if ( ticker->timer != 0 )
		return;
	ticker->timer = gtk_timeout_add(ticker->interval, 
		ticker_timeout, ticker);
}

void       
gtk_ticker_stop_scroll(GtkTicker *ticker)
{
  	g_return_if_fail (ticker != NULL);
  	g_return_if_fail (GTK_IS_TICKER (ticker));
	if ( ticker->timer == 0 )
		return;
	gtk_timeout_remove( ticker->timer );
	ticker->timer = 0;
	
}

guint
gtk_ticker_get_spacing (GtkTicker *ticker )
{
  g_return_val_if_fail (ticker != NULL, -1);
  g_return_val_if_fail (GTK_IS_TICKER (ticker), -1);

  return ticker->spacing;
}

static void
gtk_ticker_map (GtkWidget *widget)
{
  GtkTicker *ticker;
  GtkTickerChild *child;
  GList *children;

  g_return_if_fail (widget != NULL);
  g_return_if_fail (GTK_IS_TICKER (widget));

  GTK_WIDGET_SET_FLAGS (widget, GTK_MAPPED);
  ticker = GTK_TICKER (widget);

  children = ticker->children;
  while (children)
    {
      child = children->data;
      children = children->next;

      if (GTK_WIDGET_VISIBLE (child->widget) &&
	  !GTK_WIDGET_MAPPED (child->widget))
	gtk_widget_map (child->widget);
    }

  gdk_window_show (widget->window);
}

static void
gtk_ticker_realize (GtkWidget *widget)
{
  GdkWindowAttr attributes;
  gint attributes_mask;

  g_return_if_fail (widget != NULL);
  g_return_if_fail (GTK_IS_TICKER (widget));

  GTK_WIDGET_SET_FLAGS (widget, GTK_REALIZED);

  attributes.window_type = GDK_WINDOW_CHILD;
  attributes.x = widget->allocation.x;
  attributes.y = widget->allocation.y;
  attributes.width = widget->allocation.width;
  attributes.height = widget->allocation.height;
  attributes.wclass = GDK_INPUT_OUTPUT;
  attributes.visual = gtk_widget_get_visual (widget);
  attributes.colormap = gtk_widget_get_colormap (widget);
  attributes.event_mask = gtk_widget_get_events (widget);
  attributes.event_mask |= GDK_EXPOSURE_MASK | GDK_BUTTON_PRESS_MASK;

  attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;

  widget->window = gdk_window_new (gtk_widget_get_parent_window (widget), 
	&attributes, attributes_mask);
  gdk_window_set_user_data (widget->window, widget);

  widget->style = gtk_style_attach (widget->style, widget->window);
  gtk_style_set_background (widget->style, widget->window, GTK_STATE_NORMAL);
}

static void
gtk_ticker_size_request (GtkWidget      *widget,
			GtkRequisition *requisition)
{
  GtkTicker *ticker;  
  GtkTickerChild *child;
  GList *children;
  GtkRequisition child_requisition;

  g_return_if_fail (widget != NULL);
  g_return_if_fail (GTK_IS_TICKER (widget));
  g_return_if_fail (requisition != NULL);

  ticker = GTK_TICKER (widget);
  requisition->width = 0;
  requisition->height = 0;

  children = ticker->children;
  while (children)
    {
      child = children->data;
      children = children->next;

      if (GTK_WIDGET_VISIBLE (child->widget))
	{
          gtk_widget_size_request (child->widget, &child_requisition);

          requisition->height = MAX (requisition->height,
                                     child_requisition.height);
          requisition->width += child_requisition.width + ticker->spacing;
	}
    }
  if ( requisition->width > ticker->spacing )
	requisition->width -= ticker->spacing;

  requisition->height += GTK_CONTAINER (ticker)->border_width * 2;
  requisition->width += GTK_CONTAINER (ticker)->border_width * 2;
}

static void
gtk_ticker_compute_offsets (GtkTicker *ticker)
{
  GtkTickerChild *child;
  GtkRequisition child_requisition;
  GList *children;
  guint16 border_width;

  g_return_if_fail (ticker != NULL);
  g_return_if_fail (GTK_IS_TICKER(ticker));

  border_width = GTK_CONTAINER (ticker)->border_width;

  ticker->width = GTK_WIDGET(ticker)->allocation.width;
  ticker->total = 0; 
  children = ticker->children;
  while (children) {
      child = children->data;
     
      child->x = 0; 
      if (GTK_WIDGET_VISIBLE (child->widget)) {
	  gtk_widget_get_child_requisition (child->widget, &child_requisition);
	  child->offset = ticker->total;
	  ticker->total += 
		child_requisition.width + border_width + ticker->spacing;
      }
      children = children->next;
  }
  ticker->dirty = FALSE; 
}

static void
gtk_ticker_size_allocate (GtkWidget     *widget,
			 GtkAllocation *allocation)
{
  GtkTicker *ticker;
  GtkTickerChild *child;
  GtkAllocation child_allocation;
  GtkRequisition child_requisition;
  GList *children;
  guint16 border_width;

  g_return_if_fail (widget != NULL);
  g_return_if_fail (GTK_IS_TICKER(widget));
  g_return_if_fail (allocation != NULL);

  ticker = GTK_TICKER (widget);

  if ( GTK_WIDGET(ticker)->allocation.width != ticker->width )
	ticker->dirty = TRUE;

  if ( ticker->dirty == TRUE ) {
	gtk_ticker_compute_offsets( ticker );
  }

  widget->allocation = *allocation;
  if (GTK_WIDGET_REALIZED (widget))
    gdk_window_move_resize (widget->window,
			    allocation->x, 
			    allocation->y,
			    allocation->width, 
			    allocation->height);

  border_width = GTK_CONTAINER (ticker)->border_width;

  children = ticker->children;
  while (children)
    {
      child = children->data;
      child->x -= ticker->scootch;
      
      if (GTK_WIDGET_VISIBLE (child->widget)) {
	  gtk_widget_get_child_requisition (child->widget, &child_requisition);
	  child_allocation.width = child_requisition.width;
	  child_allocation.x = child->offset + border_width + child->x; 
    	  if ( ( child_allocation.x + child_allocation.width ) < GTK_WIDGET(ticker)->allocation.x  ) {
		if ( ticker->total >=  GTK_WIDGET(ticker)->allocation.width ) {
			child->x += GTK_WIDGET(ticker)->allocation.x + GTK_WIDGET(ticker)->allocation.width + ( ticker->total - ( GTK_WIDGET(ticker)->allocation.x + GTK_WIDGET(ticker)->allocation.width ) );
		}
		else {
			child->x += GTK_WIDGET(ticker)->allocation.x + GTK_WIDGET(ticker)->allocation.width;
		}
	  }
	  child_allocation.y = border_width;
	  child_allocation.height = child_requisition.height;
	  gtk_widget_size_allocate (child->widget, &child_allocation);
      }
      children = children->next;
    }
}

static void
gtk_ticker_paint (GtkWidget    *widget,
		 GdkRectangle *area)
{
  g_return_if_fail (widget != NULL);
  g_return_if_fail (GTK_IS_TICKER (widget));
  g_return_if_fail (area != NULL);

  if (GTK_WIDGET_DRAWABLE (widget))
    gdk_window_clear_area (widget->window, 0, 0, widget->allocation.width, 
	widget->allocation.height);
}

static void
gtk_ticker_draw (GtkWidget    *widget,
		GdkRectangle *area)
{
  GtkTicker *ticker;
  GtkTickerChild *child;
  GList *children;

  g_return_if_fail (widget != NULL);
  g_return_if_fail (GTK_IS_TICKER (widget));

  if (GTK_WIDGET_DRAWABLE (widget))
    {
      ticker = GTK_TICKER (widget);
      gtk_ticker_paint (widget, area);

      children = ticker->children;
      while (children)
	{
	  child = children->data;
	  children = children->next;
	    gtk_widget_draw (child->widget, NULL);
	}
    }
}

static gint
gtk_ticker_expose (GtkWidget *widget, GdkEventExpose *event)
{
  GtkTicker *ticker;
  GtkTickerChild *child;
  GdkEventExpose child_event;
  GList *children;

  g_return_val_if_fail (widget != NULL, FALSE);
  g_return_val_if_fail (GTK_IS_TICKER (widget), FALSE);
  g_return_val_if_fail (event != NULL, FALSE);

  if (GTK_WIDGET_DRAWABLE (widget))
    {
      ticker = GTK_TICKER (widget);

      child_event = *event;

      children = ticker->children;
      while (children)
	{
	  child = children->data;
	  children = children->next;

	  if (GTK_WIDGET_NO_WINDOW (child->widget) &&
	      gtk_widget_intersect (child->widget, &event->area, 
			&child_event.area))
	    gtk_widget_event (child->widget, (GdkEvent*) &child_event);
	}
    }

  return FALSE;
}

void       
gtk_ticker_add(GtkTicker *ticker, GtkWidget *widget)
{
	gtk_ticker_add_real( GTK_CONTAINER( ticker ), widget );
  	ticker->dirty = TRUE;	
}

void       
gtk_ticker_remove(GtkTicker *ticker, GtkWidget *widget)
{
	gtk_ticker_remove_real( GTK_CONTAINER( ticker ), widget );
  	ticker->dirty = TRUE;	
}

static void
gtk_ticker_add_real(GtkContainer *container,
	       GtkWidget    *widget)
{
  g_return_if_fail (container != NULL);
  g_return_if_fail (GTK_IS_TICKER (container));
  g_return_if_fail (widget != NULL);

  gtk_ticker_put(GTK_TICKER (container), widget);
}

static void
gtk_ticker_remove_real(GtkContainer *container,
		  GtkWidget    *widget)
{
  GtkTicker *ticker;
  GtkTickerChild *child;
  GList *children;

  g_return_if_fail (container != NULL);
  g_return_if_fail (GTK_IS_TICKER (container));
  g_return_if_fail (widget != NULL);

  ticker = GTK_TICKER (container);

  children = ticker->children;
  while (children)
    {
      child = children->data;

      if (child->widget == widget)
	{
	  gboolean was_visible = GTK_WIDGET_VISIBLE (widget);
	  
	  gtk_widget_unparent (widget);

	  ticker->children = g_list_remove_link (ticker->children, children);
	  g_list_free (children);
	  g_free (child);

	  if (was_visible && GTK_WIDGET_VISIBLE (container))
	    gtk_widget_queue_resize (GTK_WIDGET (container));

	  break;
	}

      children = children->next;
    }
}

static void
gtk_ticker_forall (GtkContainer *container,
		  gboolean	include_internals,
		  GtkCallback   callback,
		  gpointer      callback_data)
{
  GtkTicker *ticker;
  GtkTickerChild *child;
  GList *children;

  g_return_if_fail (container != NULL);
  g_return_if_fail (GTK_IS_TICKER (container));
  g_return_if_fail (callback != NULL);

  ticker = GTK_TICKER (container);

  children = ticker->children;
  while (children)
    {
      child = children->data;
      children = children->next;

      (* callback) (child->widget, callback_data);
    }
}
