namespace ContextProvider {

	public class StringSetIter {
		StringSet parent;
		IntSetIter iter;

		internal StringSetIter (StringSet parent) {
			this.parent = parent;
			this.iter = IntSetIter (parent.intset);
		}
		public bool next() {
			return iter.next();
		}
		public string get() {
			return ((Quark) iter.element).to_string();
		}

	}

	/* TODO:
	   we should have a way of statically initialising any keys that may be used in a stringset
	   so keys outside of this set are rejected
	   */
	/**
	 StringSet builds upon IntSet and Quarks to give us an structure which 
	 can manage sets of strings and efficiently perform set operations such
	  as intersection and union.
	 */
	public class StringSet {
		internal IntSet intset;

		StringSet.from_intset (IntSet #i) {
			intset = #i;
		}

		public StringSet () {
			intset = new IntSet();
		}

		public StringSet.sized (uint size) {
			intset = new IntSet.sized (size);
		}

		public StringSet.from_array (string[] array) {
			intset = new IntSet();
			foreach (var s in array) {
				add (s);
			}
		}

		public StringSet.intersection (StringSet left, StringSet right) {
			this.from_intset (new IntSet.intersection (left.intset, right.intset));
		}

		public StringSet.symmetric_difference (StringSet left, StringSet right) {
			this.from_intset (new IntSet.symmetric_difference (left.intset, right.intset));
		}

		public StringSet.difference (StringSet left, StringSet right) {
			this.from_intset (new IntSet.difference (left.intset, right.intset));
		}

		public StringSet.union (StringSet left, StringSet right) {
			this.from_intset (new IntSet.union (left.intset, right.intset));
		}

		public void add (string element) {
			intset.add (Quark.from_string (element));
		}

		public void clear () {
			intset.clear ();
		}
		/* TODO, not sure how @foreach functions actually work :/
		public void @foreach (IntFunc func, void* userdata) {
		}
		*/
		public bool is_member (string element) {
			Quark q = Quark.try_string (element);
			if (q != 0)
				return intset.is_member (q);
			else
				return false;
		}

		public bool is_equal (StringSet right) {
			return intset.is_equal (right.intset);
		}

		public bool is_subset (StringSet right) {
			return intset.is_subset (right.intset);
		}

		public bool is_disjoint (StringSet right) {
			return intset.is_disjoint (right.intset);
		}

		public bool remove (string element) {
			Quark q = Quark.try_string (element);
			if (q != 0)
				return intset.remove (q);
			else
				return false;
		}

		public uint size () {
			return intset.size();
		}

		public StringSetIter iterator () {
			StringSetIter iter = new StringSetIter(this);
			return iter;
		}

		public Gee.ArrayList<string> to_array () {

			IntFunc iter = (i, data) => {
				weak Gee.ArrayList<string> ret = (Gee.ArrayList<string>) data;
				ret.add (((Quark)i).to_string());
			};

			Gee.ArrayList<string> ret = new Gee.ArrayList<string>();
			intset.foreach (iter, ret);

			return ret;
		}

		public string debug () {
			Gee.ArrayList<string> strings = to_array();
			if (strings.size < 1) {
				return "";
			}

			string ret = strings[0];
			for (int i=1; i<strings.size; i++) {
				ret = ret + ", " + strings[i];
			}
			return ret;
		}

	}
}
