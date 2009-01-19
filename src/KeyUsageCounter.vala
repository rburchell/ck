using GLib;

namespace ContextKit {

	public class KeyUsageCounter {
		// The number of subscribers for each key (over all subscriber objects)
		Gee.HashMap<string, int> no_of_subscribers = new Gee.HashMap<string, int>(str_hash, str_equal);
		Providers providers;

		public KeyUsageCounter (Providers providers) {
			this.providers = providers;
		}

		public void add (StringSet keys) {
			StringSet first_subscribed_keys = new StringSet();

			foreach (var key in keys) {
				if (no_of_subscribers.contains (key)) {
					int old_value = no_of_subscribers.get (key);
					no_of_subscribers.set (key, old_value + 1);

					if (old_value == 0) {
						// This is the first subscribed to the key
						first_subscribed_keys.add (key);
					}

					debug ("Subscriber count of %s is now %d", key, old_value + 1);
				}
				else { // Key name not present in the key table
					no_of_subscribers.set (key, 1);

					first_subscribed_keys.add (key);
					debug ("Subscriber count of %s is now %d", key, 1);
				}
			}

			if (first_subscribed_keys.size() > 0) {
				// Signal that some new keys were subscribed to
				providers.first_subscribed (first_subscribed_keys);
			}

			// FIXME: Do we need to care about which keys are subscribed to? What if the keys are something not provided by this provider?
		}

		public void remove (StringSet keys) {
			StringSet last_unsubscribed_keys = new StringSet();

			foreach (var key in keys) {
				if (no_of_subscribers.contains (key)) {
					int old_value = no_of_subscribers.get (key);

					if (old_value >= 1) {
						// Do not store negative values
						no_of_subscribers.set (key, old_value - 1);
					}

					if (old_value <= 1) {
						// The last subscriber for this key unsubscribed
						last_unsubscribed_keys.add (key);
					}
					debug ("Subscriber count of %s is now %d", key, old_value - 1);
				}
				else {
					// This should not happen
					debug ("Error: decreasing the subscriber count of an unknown key.");
				}
			}

			if (last_unsubscribed_keys.size() > 0) {
				// Signal that some keys are not subscribed to anymore
				providers.last_unsubscribed (last_unsubscribed_keys);
			}
		}
	}
}


