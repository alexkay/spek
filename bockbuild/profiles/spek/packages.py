import os
from bockbuild.darwinprofile import DarwinProfile

class SpekPackages:
	def __init__ (self):
		# Toolchain
		self.packages.extend ([
			'autoconf.py',
			'automake.py',
			'libtool.py',
			'gettext.py',
			'pkg-config.py',
		])

		# Base Libraries
		self.packages.extend ([
#			'libiconv.py',
			'libpng.py',
			'libjpeg.py',
			'libxml2.py',
			'freetype.py',
			'fontconfig.py',
			'pixman.py',
			'cairo.py',
			'glib.py',
			'pango.py',
			'atk.py',
			'intltool.py',
			'gtk+.py',
			'libproxy.py',
			'libsoup.py',
		])

		# Theme
		self.packages.extend ([
			'librsvg.py',
			'icon-naming-utils.py',
			'hicolor-icon-theme.py',
#			'tango-icon-theme.py',
			'murrine.py',
		])

		# Codecs
		self.packages.extend ([
			'libogg.py',
			'libvorbis.py',
			'flac.py',
			'libtheora.py',
			'speex.py',
			'wavpack.py',
			'taglib.py',
		])

		# GStreamer
		self.packages.extend ([
			'liboil.py',
			'gstreamer.py',
			'gst-plugins-base.py',
			'gst-plugins-good.py',
			'gst-plugins-bad.py',
			'gst-plugins-ugly.py',
			'gst-ffmpeg.py',
		])

		if isinstance (self, DarwinProfile):
			self.packages.extend ([
				'ige-mac-integration.py'
			])

		if self.cmd_options.release_build:
			self.packages.append ('spek.py')

		self.packages = [os.path.join ('..', '..', 'packages', p)
			for p in self.packages]
