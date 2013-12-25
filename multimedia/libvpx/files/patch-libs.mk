
$FreeBSD: multimedia/libvpx/files/patch-libs.mk 336806 2013-12-18 09:05:36Z ashish $

--- libs.mk.orig
+++ libs.mk
@@ -232,8 +232,8 @@
 	$(qexec)echo 'Libs: -L$${libdir} -lvpx' >> $@
 	$(qexec)echo 'Libs.private: -lm -pthread' >> $@
 	$(qexec)echo 'Cflags: -I$${includedir}' >> $@
-INSTALL-LIBS-yes += $(LIBSUBDIR)/pkgconfig/vpx.pc
-INSTALL_MAPS += $(LIBSUBDIR)/pkgconfig/%.pc %.pc
+INSTALL-LIBS-yes += libdata/pkgconfig/vpx.pc
+INSTALL_MAPS += libdata/pkgconfig/%.pc %.pc
 CLEAN-OBJS += vpx.pc
 endif
 
