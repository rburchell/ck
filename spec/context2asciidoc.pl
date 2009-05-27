#!/usr/bin/perl
#
# Make a asciidoc document from spec/context.xml 
#

use XML::DOM;

sub unindent {
    # XXX - this should 'unindent' the text by removing the same
    #       amount of leading whitespace from each line so that the first
    #       line starts at the left margin.
    my $string = shift;
    $string =~ s/^\s+//;
    $string =~ s/\s+$//;
    return $string;
}

sub to_nano_dom {
    my $node = shift;

    my @children = $node->getChildNodes();

    if ($#children == 0 && $children[0]->getNodeType() == 3) {
        return unindent ($node->getFirstChild()->getNodeValue());
    } else {
        my $result = {};

        foreach ($node->getAttributes()->getValues()) {
            $result->{$_->getNodeName()} = $_->getNodeValue();
        }
        
        foreach (@children) {
            if ($_->getNodeType() == 1) {
                $result->{$_->getNodeName()} = to_nano_dom ($_);
            }
        }
        
        return $result;
    }
}

sub dump_nano_dom {
    my $level = shift;
    my $key = shift;
    my $value = shift;

    if (ref $value) {
        print $level . " " . $key . "\n";
        for my $k (keys %{$value}) {
            dump_nano_dom ($level + 1, $k, $value->{$k});
        }
    } else {
        print $level . " " . $key . ": '" . $value . "'\n";
    }
}

sub output_doc {
    my $doc = shift;
    print $doc->getFirstChild()->getNodeValue() . "\n";
}

sub type_name {
    my $type = shift;

    if (ref $type) {
        return (keys %{$type})[0];
    } else {
        return $type;
    }
}

sub type_parms {
    my $type = shift;

    if (ref ($type)) {
        return (values %{$type})[0];
    } else {
        return { };
    }
}

sub type_short_desc {
    my $type = shift;

    my $name = type_name ($type);
    my $parms = type_parms ($type);

    if ($name eq "uniform-list") {
        return "List of __" . type_parms($type)->{type} . "__s";
    } elsif ($name eq "string-enum") {
        return "Enumeration of __string__s" . type_parms($type)->{type};
    } elsif ($name eq "map") {
        return "Map";
    } else {
        return "_" . $name . "_";
    }
}

sub print_type_long_desc {
    my $type = shift;

    my $name = type_name ($type);
    my $parms = type_parms ($type);

    if ($name eq "uniform-list"
        || !%{$parms}) {
        # do nothing
    } elsif ($name eq "string-enum") {
        print "+\n--\nPossible enumeration values: \n[horizontal]\n";
        for my $key (keys %{$parms}) {
            print $key . ":: " . $parms->{$key}->{doc} . "\n";
        }
        print "--\n";
    } elsif ($name eq "map") {
        print "+\n--\nPossible map keys: \n[horizontal]\n";
        my $keys = $parms->{'allowed-keys'};
        for my $key (keys %{$keys}) {
            print $key . ":: " . $keys->{$key}->{doc} . "\n";
        }
        print "--\n";
    } else {
        print "+\n--\nType: " . type_name ($type) . "\n[horizontal]\n";
        for my $key (keys %{$parms}) {
            print "$key :: $parms->{$key}\n";
        }
        print "--\n";
    }
}

sub output_key {
    my $key = to_nano_dom (shift);

    # dump_nano_dom (0, "Key", $key);

    print "\n";
    print "*" . $key->{name} . "* (" . type_short_desc ($key->{type}) . ")::\n";
    print $key->{doc} . "\n";
    print_type_long_desc ($key->{type});
}

my $document = XML::DOM::Parser->new()->parse(STDIN);

foreach ($document->getDocumentElement()->getChildNodes()) {
    my $node = $_;
    if ($node->getNodeName() eq "doc") {
        output_doc ($node);
    } elsif ($node->getNodeName() eq "key") {
        output_key ($node);
    }
}
