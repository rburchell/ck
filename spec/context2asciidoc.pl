#!/usr/bin/perl
#
# Make a asciidoc document from spec/context.xml 
#

use XML::LibXML::Reader;

$reader = new XML::LibXML::Reader(FD => STDIN)
      or die "cannot read standard input?";

$depth = 0;
while ($reader->read) {
  if ($reader->name eq "node") {          # keep track of our position in the context tree
    if ($reader->nodeType == XML_READER_TYPE_ELEMENT) {
      $name[$depth] = $reader->getAttribute(name); 
      $id[$depth]++;
      $depth++ unless $reader->isEmptyElement;
    }  elsif ($reader->nodeType == XML_READER_TYPE_END_ELEMENT) {
      $depth--;
      if (!$firsttime++) {
        pop @name;
        pop @id;
      }
    }
  } elsif ($reader->name eq "key") {      # print keys with type
    if ($reader->nodeType == XML_READER_TYPE_ELEMENT) {
      print "*" . join('.', @name) . "." . $reader->getAttribute(name)
	    . "* (_" . $reader->getAttribute(type) . "_)::\n";
    } elsif ($reader->nodeType == XML_READER_TYPE_END_ELEMENT) {
      print "\n";
    }
  } elsif ($reader->name eq "doc" && $reader->nodeType == XML_READER_TYPE_ELEMENT) {
    # print the related documentation
    print $reader->readInnerXml() . "\n";
  }
}
