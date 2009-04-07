namespace ContextProvider {
	public class Group : GLib.Object {
		public SubscriptionChangedCallback? callback;
		public StringSet keys;
		bool subscribed;
		bool clear_values_on_subscribe;

		public signal void clear_values(StringSet keys);

		public Group (string[] keys, bool clear_values_on_subscribe, SubscriptionChangedCallback? cb) {
			this.keys = new StringSet.from_array(keys);
			this.clear_values_on_subscribe = clear_values_on_subscribe;
			this.callback = cb;
			this.subscribed = false;
		}
		public void subscribe (bool subscribed) {
			if (subscribed != this.subscribed) {
				this.subscribed = subscribed;
				if (callback != null)
					callback (subscribed);
				if (clear_values_on_subscribe && subscribed) {
					clear_values(keys);
				}
			}
		}
	}
}
