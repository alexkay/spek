class SpekPackage (Package):
	def __init__ (self):
		Package.__init__ (self, 'spek', '0.4')

		self.sources = [
			'http://%{name}.googlecode.com/files/%{name}-%{version}.tar.bz2'
		]

SpekPackage ()
