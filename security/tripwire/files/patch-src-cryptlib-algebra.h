--- src/cryptlib/algebra.h.orig	2011-11-21 17:06:56.000000000 +0100
+++ src/cryptlib/algebra.h	2014-06-15 01:42:42.000000000 +0200
@@ -273,7 +273,7 @@ template <class T> T AbstractEuclideanDo
 	Element g[3]={b, a};
 	unsigned int i0=0, i1=1, i2=2;
 
-	while (!Equal(g[i1], this->Zero()))
+	while (!this->Equal(g[i1], this->Zero()))
 	{
 		g[i2] = Mod(g[i0], g[i1]);
 		unsigned int t = i0; i0 = i1; i1 = i2; i2 = t;
