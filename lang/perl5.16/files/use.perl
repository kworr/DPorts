#!/bin/sh

# $FreeBSD: lang/perl5.16/files/use.perl 327989 2013-09-23 12:37:58Z az $

this=`echo -n $0 | /usr/bin/sed -e 's!^.*/!!'`
PERL_VERSION="%%PERL_VERSION%%"
PERL_VER="%%PERL_VER%%"
PERL5_SIGNATURE=%%PERL5_SIGNATURE%%
banner=`date +"%F %T"`
banner="# added by use.perl $banner"
if [ -z "${OSVERSION}" ]; then
	if [ -f /sbin/sysctl -a -x /sbin/sysctl ] ; then
    		osreldate=`/sbin/sysctl -n kern.osreldate`
	else
    		osreldate=`/usr/sbin/sysctl -n kern.osreldate`
	fi
else
    	osreldate=${OSVERSION}
fi

if [ "$2" = "POST-INSTALL" ] ; then
	need_remove_links=%%LINK_USRBIN%%
	need_create_links=%%LINK_USRBIN%%
	need_remove_perl5_signature=yes
	need_create_perl5_signature=yes
	need_post_install=yes
	if [ "${osreldate}" -lt 900022 ]; then
		need_cleanup_manpath=yes
		need_spam_manpath=yes
	fi
elif [ "$2" = "POST-DEINSTALL" ] ; then
	need_remove_links=%%LINK_USRBIN%%
	need_remove_perl5_signature=yes
	if [ "${osreldate}" -lt 900022 ]; then
		need_cleanup_manpath=yes
	fi
else
	exit 0;
fi

special_link_list="
	perl
	perl5"

do_remove_links()
{
	echo "Removing stale symlinks from /usr/bin..."
	for binary in $special_link_list
	do
		if [ -L "/usr/bin/$binary" ] ; then
			echo "    Removing /usr/bin/$binary"
			/bin/rm -f "/usr/bin/$binary"
		else
			echo "    Skipping /usr/bin/$binary"
		fi
	done
	bins=`/bin/ls /usr/bin/*perl*5.* ${PKG_PREFIX}/bin/*perl*5.* 2>/dev/null`
	for binary in $bins
	do
		if [ -L "$binary" ] ; then
			echo "    Removing $binary installed by an older perl port"
			/bin/rm -f "$binary"
		fi
	done
	echo "Done."
}

do_create_links()
{
	echo "Creating various symlinks in /usr/bin..."
	for binary in $special_link_list
	do
		if [ -f "/usr/bin/$binary" ] ; then
			echo "    Backing up /usr/bin/$binary as /usr/bin/$binary.freebsd"
			/bin/mv -f "/usr/bin/$binary" "/usr/bin/$binary.freebsd"
		fi
		bin=`echo $binary | /usr/bin/sed -e 's!perl5!perl!'`
		if [ -e "/usr/bin/$binary" ] ; then
			echo "    *** /usr/bin/$binary is still there, which should not happen"
		elif [ -e "$PKG_PREFIX/bin/${bin}%%PERL_VERSION%%" ] ; then
			echo "    Symlinking $PKG_PREFIX/bin/${bin}%%PERL_VERSION%% to /usr/bin/$binary"
			/bin/ln -sf "$PKG_PREFIX/bin/${bin}%%PERL_VERSION%%" "/usr/bin/$binary"
		else
			echo "    *** $PKG_PREFIX/bin/${bin}%%PERL_VERSION%% is not there, a symlink won't do any good"
		fi
	done
	echo "Done."
}

do_post_install()
{
	INCLUDEDIR=/usr/include
	install -d ${PKG_PREFIX}/lib/perl5/site_perl/%%PERL_VER%%/%%PERL_ARCH%%/auto
	install -d ${PKG_PREFIX}/lib/perl5/site_perl/%%PERL_VER%%/auto
	install -d ${PKG_PREFIX}/lib/perl5/%%PERL_VER%%/man/man3
	cd ${INCLUDEDIR} && ${PKG_PREFIX}/bin/h2ph *.h machine/*.h sys/*.h >/dev/null
}

do_remove_perl5_signature()
{
	echo -n "Removing ${PERL5_SIGNATURE}..."
	if [ -f ${PERL5_SIGNATURE} ] ; then
		/bin/rm ${PERL5_SIGNATURE}
	fi
	echo " Done."
}

do_cleanup_manpath()
{
	if [ -f /etc/manpath.config ] ; then
		echo -n "Cleaning up /etc/manpath.config..."
		/bin/cp -p /etc/manpath.config /etc/manpath.config.new
		/usr/bin/awk 's=0;
			/^#.*use.perl/ { s=1; mode=1 }
			/^#/ { s=1; if (mode != 1) { mode=0 } }
			/^OPTIONAL_MANPATH.*perl5/ { s=1; if (mode == 1) { mode=2 } }
			/^$/ { s=1; if (mode != 2) { mode = 0 } }
			{ if (s != 1) { mode = 0 } if (mode == 0) print }' /etc/manpath.config >/etc/manpath.config.new
		/bin/mv /etc/manpath.config /etc/manpath.config.bak
		/bin/mv /etc/manpath.config.new /etc/manpath.config
		echo " Done."
	fi
}

do_create_perl5_signature()
{
	echo -n "Creating ${PERL5_SIGNATURE}..."
	echo "$banner" >>${PERL5_SIGNATURE}
	echo "# Do not modify PERL_VERSION here, instead use DEFAULT_VERSIONS= perl5=${PERL_VER}" >>${PERL5_SIGNATURE}
	echo "PERL_VERSION=%%PERL_VERSION%%" >>${PERL5_SIGNATURE}
	echo " Done."
}

do_spam_manpath()
{
	if [ -f /etc/manpath.config ] ; then
		echo -n "Spamming /etc/manpath.config..."
		echo "$banner" >>/etc/manpath.config
		echo "OPTIONAL_MANPATH	${PKG_PREFIX}/lib/perl5/%%PERL_VER%%/man" >>/etc/manpath.config
		echo "OPTIONAL_MANPATH	${PKG_PREFIX}/lib/perl5/%%PERL_VER%%/perl/man" >>/etc/manpath.config
		echo " Done."
	fi
}

[ "$need_remove_links" = "yes" ] && do_remove_links
[ "$need_create_links" = "yes" ] && do_create_links
[ "$need_post_install" = "yes" ] && do_post_install
[ "$need_remove_perl5_signature" = "yes" ] && do_remove_perl5_signature
[ "$need_create_perl5_signature" = "yes" ] && do_create_perl5_signature
[ "$need_cleanup_manpath" = "yes" ] && do_cleanup_manpath
[ "$need_spam_manpath" = "yes" ] && do_spam_manpath 

exit 0
