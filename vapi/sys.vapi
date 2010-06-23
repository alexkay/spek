[CCode (cheader_filename = "sys/utsname.h")]
namespace Sys {
	[CCode (cname = "struct utsname")]
	public struct UtsName {
		public weak string sysname;
		public weak string nodename;
		public weak string release;
		public weak string version;
		public weak string machine;
		public weak string domainname;
	}
	[CCode (cname = "uname")]
	public int uname (ref UtsName name);
}