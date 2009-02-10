namespace ContextProvider {

	/**
	 * SECTION:string_set
	 * @short_description: An unorder efficient set of strings
	 * @include: string_set.h
	 * @see_also: <link linkend="libcontextprovider-Context-provider-setup">Context Provider</link>
	 *
	 * A data structure which efficiently holds a set of strings. A string set has a maximum memory usage
	 * of (total number of strings stored)/8 bytes.
	 * All operations other than iteration are O(1)
	 */

	public class StringSet {
	/**
	 * ContextProviderStringSet:
	 *
	 * Structure representing a string set
	 */
		private IntSet intset;

		StringSet.from_intset (IntSet #intset) {
			this.intset = #intset;
		}

		/**
		 * context_provider_string_set_new:
		 *
		 * Create a new #ContextProviderStringSet
		 *
		 * Returns: a new #ContextProviderStringSet
		 */
		public StringSet () {
			/* TODO:
			   we should have a way of statically initialising any keys that may be used in a stringset
			   so keys outside of this set are rejected
			   */


			intset = new IntSet();
		}

		/**
		 * context_provider_string_set_new_sized:
		 *
		 * Create a new #ContextProviderStringSet with an initially allocated size.
		 *
		 * Returns: a new #ContextProviderStringSet
		 */
		public StringSet.sized (uint size) {
			intset = new IntSet.sized (size);
		}

		/**
		 * context_provider_string_set_new_from_array:
		 * @array: an exising array of strings, null terminated
		 *
		 * Create a new #ContextProviderStringSet from an existing string array.
		 *
		 * Returns: a new #ContextProviderStringSet
		 */
		public StringSet.from_array ([CCode (array_length = false, array_null_terminated = true)] string[] array) {
			intset = new IntSet();
			foreach (var s in array) {
				add (s);
			}
		}

		/**
		 * context_provider_string_set_new_intersection:
		 * @left: a #ContextProviderStringSet
		 * @right: a #ContextProviderStringSet
		 *
		 * Create a new #ContextProviderStringSet by intersecting two existing string sets.
		 *
		 * Returns: a new #ContextProviderStringSet
		 */
		public StringSet.intersection (StringSet left, StringSet right) {
			this.from_intset (new IntSet.intersection (left.intset, right.intset));
		}

		/**
		 * context_provider_string_set_new_symmetric_difference:
		 * @left: a #ContextProviderStringSet
		 * @right: a #ContextProviderStringSet
		 *
		 * Create a new #ContextProviderStringSet by finding the symmetric difference
		 * between two existing string sets.
		 * Symmetric difference gives you the elements that are in either set, but not in both
		 *
		 * Returns: a new #ContextProviderStringSet
		 */
		public StringSet.symmetric_difference (StringSet left, StringSet right) {
			this.from_intset (new IntSet.symmetric_difference (left.intset, right.intset));
		}

		/**
		 * context_provider_string_set_new_difference:
		 * @left: a #ContextProviderStringSet
		 * @right: a #ContextProviderStringSet
		 *
		 * Create a new #ContextProviderStringSet by finding the difference
		 * between two existing string sets.
		 * Difference gives you the elements that are in #left, but not in #right.
		 *
		 * Returns: a new #ContextProviderStringSet
		 */
		public StringSet.difference (StringSet left, StringSet right) {
			this.from_intset (new IntSet.difference (left.intset, right.intset));
		}

		/**
		 * context_provider_string_set_new_union:
		 * @left: a #ContextProviderStringSet
		 * @right: a #ContextProviderStringSet
		 *
		 * Create a new #ContextProviderStringSet by finding the union
		 * of two existing string sets.
		 *
		 * Returns: a new #ContextProviderStringSet
		 */
		public StringSet.union (StringSet left, StringSet right) {
			this.from_intset (new IntSet.union (left.intset, right.intset));
		}

		/**
		 * context_provider_string_set_add:
		 * @self: a #ContextProviderStringSet
		 * @element: a string
		 *
		 * Add a string to a stringset
		 */
		public void add (string element) {
			intset.add (Quark.from_string (element));
		}

		/**
		 * context_provider_string_set_remove:
		 * @self: a #ContextProviderStringSet
		 * @element: a string
		 *
		 * Remove a string from a stringset
		 *
		 * Returns: TRUE if #element was a member of #self
		 */
		public bool remove (string element) {
			Quark q = Quark.try_string (element);
			if (q != 0)
				return intset.remove (q);
			else
				return false;
		}

		/**
		 * context_provider_string_set_size:
		 * @self: a #ContextProviderStringSet
		 *
		 * Find the number of elements in a string set.
		 *
		 * Returns: the number of elements in #self.
		 */
		public uint size () {
			return intset.size();
		}


		/**
		 * context_provider_string_set_clear:
		 * @self: a #ContextProviderStringSet
		 *
		 * Remove all elements from a string set.
		 */
		public void clear () {
			intset.clear ();
		}

		/**
		 * context_provider_string_set_is_member:
		 * @self: a #ContextProviderStringSet
		 * @element: a string
		 *
		 * Find if #element is a member of the set.
		 *
		 * Returns: TRUE if #element is a member of #self
		 */
		public bool is_member (string element) {
			Quark q = Quark.try_string (element);
			if (q != 0)
				return intset.is_member (q);
			else
				return false;
		}

		/**
		 * context_provider_string_set_is_equal:
		 * @self: a #ContextProviderStringSet
		 * @string_set: a #ContextProviderStringSet
		 *
		 * Find if #string_set contains exactly the same elements as #self
		 *
		 * Returns: TRUE if #string_set is equivalent #self
		 */
		public bool is_equal (StringSet string_set) {
			return intset.is_equal (string_set.intset);
		}


		/**
		 * context_provider_string_set_is_subset_of:
		 * @self: a #ContextProviderStringSet
		 * @string_set: a #ContextProviderStringSet
		 *
		 * Find if all the elements of #self are contained in #string_set
		 *
		 * Returns: TRUE if #string_set is a subset of #self
		 */
		public bool is_subset_of (StringSet string_set) {
			return intset.is_subset_of (string_set.intset);
		}

		/**
		 * context_provider_string_set_is_disjoint:
		 * @self: a #ContextProviderStringSet
		 * @string_set: a #ContextProviderStringSet
		 *
		 * Find if #string_set contains exactly no elements in common with #self
		 *
		 * Returns: TRUE if #string_set is disjoint with #self
		 */
		public bool is_disjoint (StringSet string_set) {
			return intset.is_disjoint (string_set.intset);
		}

		/**
		 * ContextProviderStringSetIter:
		 *
		 * A structure used for iterating over a #ContextKitStringSet.
		 */
		public class Iter {
			StringSet parent;
			IntSetIter iter;

			private Iter (StringSet parent) {
				this.parent = parent;
				this.iter = IntSetIter (parent.intset);
			}
			/**
			 * context_provider_string_set_iter_next:
			 * @self: a #ContextProviderStringSetIter
			 *
			 * Move the iterator onto the next element in the string set
			 *
			 * Returns: FALSE if there are no more elements in the set, TRUE otherwise.
			 */
			public bool next() {
				return iter.next();
			}

			/**
			 * context_provider_string_set_iter_get:
			 * @self: a #ContextProviderStringSetIter
			 *
			 * Get the string currently pointed to by this iterator
			 *
			 * Returns: a string from the string set
			 */
			public string get() {
				return ((Quark) iter.element).to_string();
			}

		}


		/**
		 * context_provider_string_set_iterator:
		 * @self: a #ContextProviderStringSet
		 *
		 * Return a #ContextProviderStringSetIter onto @self.
		 *
		 * Returns: a ContextProviderStringSetIter
		 */
		public StringSet.Iter iterator () {
			StringSet.Iter iter = new StringSet.Iter(this);
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

		/**
		 * context_provider_string_set_debug:
		 * @self: a #ContextProviderStringSet
		 *
		 * Used to print the contents of a stringset.
		 *
		 * Returns: a string with the contents of "#self comma seperated.
		 */
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
