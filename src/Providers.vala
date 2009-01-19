using GLib;

namespace ContextKit {

	public class Providers {
		// List of providers
		Gee.ArrayList<Provider> providers = new Gee.ArrayList<Provider> ();

		// Valid keys provided by all providers
		public StringSet valid_keys = new StringSet();

		public void register_provider(string[] keys, Provider provider) {
			providers.add (provider);
			valid_keys = new StringSet.union (valid_keys, new StringSet.from_array (keys));
		}

		public List<string> get (StringSet keys, HashTable<string, Value?> values) {
			List<string> unavail = new List<string> ();
			foreach (var provider in providers) {
				provider.Get (keys, values, ref unavail);
			}
			return unavail;
		}

		public void first_subscribed(StringSet keys) {
			foreach (var provider in providers) {
				provider.KeysSubscribed (keys);
			}
		}

		public void last_unsubscribed(StringSet keys) {
			foreach (var provider in providers) {
				provider.KeysUnsubscribed (keys);
			}
		}
	}
}
