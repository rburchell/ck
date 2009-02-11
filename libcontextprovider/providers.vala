using Gee;

namespace ContextProvider {

	public class Providers {
		// List of providers
		Gee.ArrayList<Provider> providers = new Gee.ArrayList<Provider> ();

		// Valid keys provided by all providers
		public StringSet valid_keys {get; private set;}

		internal Providers () {
			valid_keys = new StringSet();
		}

		public void register (Provider provider) {
			providers.add (provider);
			valid_keys = new StringSet.union (valid_keys, provider.provided_keys());
		}

		public void unregister (Provider provider) {
			providers.remove (provider);
			StringSet s = new StringSet();

			/* a bit inefficient, but it'll do for now...
			   TODO: make the stringset stuff not use ctors..
			 */
			foreach (var p in providers) {
				s = new StringSet.union (s, p.provided_keys());
			}
			valid_keys = s;
		}

		public ArrayList<string> get (StringSet keys, HashTable<string, Value?> values) {
			ArrayList<string> unavail = new ArrayList<string> ();
			foreach (var provider in providers) {
				StringSet intersection = new StringSet.intersection (keys, provider.provided_keys());
				if (intersection.size() > 0) {
					provider.get (keys, values, unavail);
				}
			}
			return unavail;
		}

		public void first_subscribed(StringSet keys) {
			foreach (var provider in providers) {
				StringSet intersection = new StringSet.intersection (keys, provider.provided_keys());
				if (intersection.size() > 0 ) {
					provider.keys_subscribed (keys);
				}
			}
		}

		public void last_unsubscribed(StringSet keys_unsubscribed, StringSet keys_remaining) {
			foreach (var provider in providers) {
				StringSet intersection = new StringSet.intersection (keys_unsubscribed, provider.provided_keys());
				if (intersection.size() > 0 ) {
					StringSet intersection_remaining = new StringSet.intersection (keys_remaining, provider.provided_keys());
					provider.keys_unsubscribed (intersection, intersection_remaining);
				}
			}
		}
	}
}
