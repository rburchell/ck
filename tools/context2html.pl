#!/usr/bin/perl
#
# Make simple HTML docs from spec/context.xml 
#

use XML::LibXML::Reader;

$reader = new XML::LibXML::Reader(FD => STDIN)
      or die "cannot read standard input?";

print "<html><body>\n<h1>Contextual Properties</h1>\n";

$depth = 0;
while ($reader->read) {
  if ($reader->name eq "node") {          # keep track of our position in the context tree
    if ($reader->nodeType == XML_READER_TYPE_ELEMENT) {
      $name[$depth] = $reader->getAttribute(name); 
      $id[$depth]++;
      $depth++ unless $reader->isEmptyElement;
      print "<h2>" . join('.', @id) . " " . join('.', @name) . "</h2>\n";
      $firstKey = true;
    }  elsif ($reader->nodeType == XML_READER_TYPE_END_ELEMENT) {
      $depth--;
      if (!$firsttime++) {
        pop @name;
        pop @id;
      }
    }
  } elsif ($reader->name eq "key") {      # print keys with type
    if ($reader->nodeType == XML_READER_TYPE_ELEMENT) {
      if ($firstkey) {
        print "<ul>\n";
        $firstkey = false;
      }
      print "<li><strong>" . $reader->getAttribute(name) . " (" . $reader->getAttribute(type). ")</strong> ";
      $inKey = !$reader->isEmptyElement;
      print  "</li>\n" unless ($inKey);
    } elsif ($reader->nodeType == XML_READER_TYPE_END_ELEMENT) {
      print "</li>\n";
      $inKey = 0;
    }
  } elsif ($reader->name eq "doc" && $reader->nodeType == XML_READER_TYPE_ELEMENT) {
    # print the related documentation
    print " -- " if ($inKey); 
    print $reader->readInnerXml() . "\n";
    print "<p>" unless ($inKey);
  }
}

print "</body></html>\n";
