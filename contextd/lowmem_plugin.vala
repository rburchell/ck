using Posix;
using GLib;

namespace Plugins {

	private enum Tristate {
		FALSE,
		TRUE,
		UNKNOWN
	}

	// Some rules for using poll(2) with sysfs:
	// -- it is not supported by every attribute
	// -- it reports POLLPRI | POLLER when something interesting happens
	// -- you have to lseek(2) to the beginning after reading it
	private class BoolSysfsPoller: Object {

		public signal void value_changed();

		public Tristate value;
		public string key;

		private IOChannel ioc;

		private bool ioc_ready(IOChannel source, IOCondition condition) {
			if ((condition & (IOCondition.PRI | IOCondition.ERR)) !=
			    (IOCondition.PRI | IOCondition.ERR))
				return true;
			Tristate new_state;
			try {
				char[] buf = new char[32];
				size_t bread;

				source.read_chars(buf, out bread);
				if (buf[0] == '0')
					new_state = Tristate.FALSE;
				else if (buf[0] == '1')
					new_state = Tristate.TRUE;
				else
					new_state = Tristate.UNKNOWN;
				source.seek_position(0, SeekType.SET);
			} catch {
				new_state = Tristate.UNKNOWN;
			}
			if (new_state != value) {
				value = new_state;
				value_changed();
			}
			return true;
		}

		BoolSysfsPoller(string filename, string key) {
			this.key = key;
			this.value = Tristate.UNKNOWN;
			this.ioc = new IOChannel.file(filename, "r");
			this.ioc.set_encoding(null);
			this.ioc.set_buffered(false);
			this.ioc.add_watch(IOCondition.PRI | IOCondition.ERR, ioc_ready);
		}
	}

	internal class LowmemPlugin: Object, ContextD.Plugin {
		/*
		 * In Maemo terms:
		 * -- low_watermark is bgkill
		 * -- high_watermark is lowmem (expect the oom killer soon)
		 */

		private struct Property {
			public string key;
			public string sysfs_attr;
			public BoolSysfsPoller poller;
			Property(string key, string sysfs_attr) {
				this.key = key;
				this.sysfs_attr = sysfs_attr;
			}
		}

		private Property[] properties = {
			Property("System.MemoryLow", "/sys/kernel/low_watermark"),
			Property("System.OomKillerApproaching", "/sys/kernel/high_watermark")
		};

		private void attr_changed(BoolSysfsPoller sp) {
			if (sp.value == Tristate.UNKNOWN)
				ContextProvider.set_null(sp.key);
			else
				ContextProvider.set_boolean(sp.key, sp.value == Tristate.TRUE);
		}

		bool install() {
			foreach (var p in properties)
				ContextProvider.install_key(p.key, false, null);
			return true;
		}

		internal LowmemPlugin() {
			foreach (var p in properties) {
				p.poller = new BoolSysfsPoller(p.sysfs_attr, p.key);
				p.poller.value_changed.connect(attr_changed);
			}
		}

	}
}
