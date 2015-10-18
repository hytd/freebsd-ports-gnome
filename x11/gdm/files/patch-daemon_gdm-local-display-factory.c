--- daemon/gdm-local-display-factory.c.orig	2015-09-21 16:12:33.000000000 +0200
+++ daemon/gdm-local-display-factory.c	2015-10-18 16:48:51.256655000 +0200
@@ -42,6 +42,7 @@
 
 #define GDM_LOCAL_DISPLAY_FACTORY_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), GDM_TYPE_LOCAL_DISPLAY_FACTORY, GdmLocalDisplayFactoryPrivate))
 
+#define CK_SEAT1_PATH                       "/org/freedesktop/ConsoleKit/Seat1"
 #define SYSTEMD_SEAT0_PATH                  "seat0"
 
 #define GDM_DBUS_PATH                       "/org/gnome/DisplayManager"
@@ -59,8 +60,10 @@ struct GdmLocalDisplayFactoryPrivate
         /* FIXME: this needs to be per seat? */
         guint            num_failures;
 
+#ifdef WITH_SYSTEMD
         guint            seat_new_id;
         guint            seat_removed_id;
+#endif
 };
 
 enum {
@@ -190,8 +193,20 @@ store_display (GdmLocalDisplayFactory *f
 static const char *
 get_seat_of_transient_display (GdmLocalDisplayFactory *factory)
 {
+        const char *seat_id = NULL;
+
         /* FIXME: don't hardcode seat */
-        return SYSTEMD_SEAT0_PATH;
+#ifdef WITH_SYSTEMD
+        if (LOGIND_RUNNING() > 0) {
+                seat_id = SYSTEMD_SEAT0_PATH;
+        }
+#endif
+
+        if (seat_id == NULL) {
+                seat_id = CK_SEAT1_PATH;
+        }
+
+        return seat_id;
 }
 
 /*
@@ -216,7 +231,19 @@ gdm_local_display_factory_create_transie
 
         g_debug ("GdmLocalDisplayFactory: Creating transient display");
 
-        display = gdm_local_display_new ();
+#ifdef WITH_SYSTEMD
+        if (LOGIND_RUNNING() > 0) {
+                display = gdm_local_display_new ();
+        }
+#endif
+
+        if (display == NULL) {
+                guint32 num;
+
+                num = take_next_display_number (factory);
+
+                display = gdm_legacy_display_new (num);
+        }
 
         seat_id = get_seat_of_transient_display (factory);
         g_object_set (display,
@@ -290,7 +317,7 @@ on_display_status_changed (GdmDisplay   
                         /* reset num failures */
                         factory->priv->num_failures = 0;
 
-                        gdm_local_display_factory_sync_seats (factory);
+                        create_display (factory, seat_id, session_type, is_initial);
                 }
                 break;
         case GDM_DISPLAY_FAILED:
@@ -372,12 +399,14 @@ create_display (GdmLocalDisplayFactory *
         g_debug ("GdmLocalDisplayFactory: Adding display on seat %s", seat_id);
 
 
+#ifdef WITH_SYSTEMD
         if (g_strcmp0 (seat_id, "seat0") == 0) {
                 display = gdm_local_display_new ();
                 if (session_type != NULL) {
                         g_object_set (G_OBJECT (display), "session-type", session_type, NULL);
                 }
         }
+#endif
 
         if (display == NULL) {
                 guint32 num;
@@ -402,6 +431,8 @@ create_display (GdmLocalDisplayFactory *
         return display;
 }
 
+#ifdef WITH_SYSTEMD
+
 static void
 delete_display (GdmLocalDisplayFactory *factory,
                 const char             *seat_id) {
@@ -538,6 +569,7 @@ gdm_local_display_factory_stop_monitor (
                 factory->priv->seat_removed_id = 0;
         }
 }
+#endif
 
 static void
 on_display_added (GdmDisplayStore        *display_store,
@@ -576,6 +608,7 @@ static gboolean
 gdm_local_display_factory_start (GdmDisplayFactory *base_factory)
 {
         GdmLocalDisplayFactory *factory = GDM_LOCAL_DISPLAY_FACTORY (base_factory);
+        GdmDisplay		*display;
         GdmDisplayStore *store;
 
         g_return_val_if_fail (GDM_IS_LOCAL_DISPLAY_FACTORY (factory), FALSE);
@@ -592,8 +625,17 @@ gdm_local_display_factory_start (GdmDisp
                           G_CALLBACK (on_display_removed),
                           factory);
 
-        gdm_local_display_factory_start_monitor (factory);
-        return gdm_local_display_factory_sync_seats (factory);
+#ifdef WITH_SYSTEMD
+	if (LOGIND_RUNNING()) {
+        	gdm_local_display_factory_start_monitor (factory);
+        	return gdm_local_display_factory_sync_seats (factory);
+	}
+#endif
+
+	/* On ConsoleKit just create Seat1, and that's it. */
+        display = create_display (factory, CK_SEAT1_PATH, NULL, TRUE);
+
+        return display != NULL;
 }
 
 static gboolean
@@ -604,17 +646,20 @@ gdm_local_display_factory_stop (GdmDispl
 
         g_return_val_if_fail (GDM_IS_LOCAL_DISPLAY_FACTORY (factory), FALSE);
 
+#ifdef WITH_SYSTEMD
         gdm_local_display_factory_stop_monitor (factory);
+#endif
 
         store = gdm_display_factory_get_display_store (GDM_DISPLAY_FACTORY (factory));
 
+#ifdef WITH_SYSTEMD
         g_signal_handlers_disconnect_by_func (G_OBJECT (store),
                                               G_CALLBACK (on_display_added),
                                               factory);
         g_signal_handlers_disconnect_by_func (G_OBJECT (store),
                                               G_CALLBACK (on_display_removed),
                                               factory);
-
+#endif
         return TRUE;
 }
 
@@ -760,7 +805,9 @@ gdm_local_display_factory_finalize (GObj
 
         g_hash_table_destroy (factory->priv->used_display_numbers);
 
+#ifdef WITH_SYSTEMD
         gdm_local_display_factory_stop_monitor (factory);
+#endif
 
         G_OBJECT_CLASS (gdm_local_display_factory_parent_class)->finalize (object);
 }
