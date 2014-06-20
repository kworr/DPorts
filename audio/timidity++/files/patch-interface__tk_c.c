--- interface/tk_c.c.orig
+++ interface/tk_c.c
@@ -913,7 +913,7 @@
 	vsnprintf(buf, sizeof(buf), fmt, ap);
 	Tcl_Eval(my_interp, buf);
 	va_end(ap);
-	return my_interp->result;
+	return Tcl_GetStringResult(my_interp);
 }
 
 static const char *v_get2(const char *v1, const char *v2)
