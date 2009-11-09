#!/usr/bin/perl
#
# Make a asciidoc document from spec/core.context or spec/core.types
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
        return [ unindent ($node->getFirstChild()->getNodeValue()) ];
    } else {
        my $result = [];

        foreach ($node->getAttributes()->getValues()) {
            push (@{$result}, [$_->getNodeName(), $_->getNodeValue()]);
        }
        
        foreach (@children) {
            if ($_->getNodeType() == 1) {
                my @elt = ($_->getNodeName(), @{to_nano_dom ($_)});
                push (@{$result}, \@elt);
            }
        }
        
        return $result;
    }
}

sub car {
    my ($nano) = @_;
    return @{$nano}[0];
}

sub cdr {
    my ($nano) = @_;
    # XXX - one free beer if you can express this array slices.
    my $res = [];
    for ($i = 1; $i < @{$nano}; $i++) {
        push (@{$res}, $nano->[$i]);
    }
    return $res;
}

sub nano_assoc {
    my ($nano, $key) = @_;

    if (ref $nano) {
        foreach (@{$nano}) {
            if (ref $_ && car ($_) eq $key) {
                return cdr ($_);
            }
        }
    }
    
    return nil;
}

sub nano_ref {
    my ($nano, $key) = @_;

    return car (nano_assoc($nano, $key));
}

sub pk1 {
    my ($v) = @_;

    if (ref $v) {
        print STDERR "[ ";
        foreach (@{$v}) {
            pk1 ($_);
            print STDERR ", ";
        }
        print STDERR "]";
    } else {
        print STDERR $v;
    }
}

sub pk {
    my ($l,$v) = @_;
    print STDERR "$l: ";
    pk1 ($v);
    print STDERR "\n";
}

sub output_doc {
    my $doc = shift;
    print $doc->getFirstChild()->getNodeValue() . "\n";
}

sub type_name {
    my $type = shift;

    if (ref $type) {
        return car ($type);
    } else {
        return $type;
    }
}

sub type_parms {
    my $type = shift;

    if (ref $type) {
        return cdr ($type);
    } else {
        return [];
    }
}

sub type_short_desc {
    my $type = shift;

    my $name = type_name ($type);
    my $parms = type_parms ($type);

    if ($name eq "uniform-list") {
        return "List of __" . nano_ref ($parms, 'type') . "__s";
    } elsif ($name eq "string-enum") {
        return "Enumeration of __string__s" . nano_ref ($parms, 'type');
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
        || !@{$parms}) {
        # do nothing
    } elsif ($name eq "string-enum") {
        print "+\n--\nPossible enumeration values: \n[horizontal]\n";
        foreach (@{$parms}) {
            print car ($_) . ":: " . nano_ref (cdr ($_), 'doc') . "\n";
        }
        print "--\n";
    } elsif ($name eq "map") {
        print "+\n--\nPossible map keys: \n[horizontal]\n";
        my $keys = nano_assoc ($parms, 'keys');
        foreach (@{$keys}) {
            print car ($_) . ":: " . nano_ref (cdr ($_), 'doc') . "\n";
        }
        print "--\n";
    } else {
        print "+\n--\nType: " . type_name ($type) . "\n[horizontal]\n";
        foreach (@{$parms}) {
            print car ($_) . " :: " . cdr ($_) . "\n";
        }
        print "--\n";
    }
}

sub output_key {
    my $key = to_nano_dom (shift);
    my $type = nano_ref ($key, 'type');

    print "\n";
    print "*" . nano_ref ($key, 'name') . "* (" . type_short_desc ($type) . ")::\n";
    print nano_ref ($key, 'doc') . "\n";
    print_type_long_desc ($type);
}

sub output_typedef {
    my $typedef = to_nano_dom (shift);

    print "\n";
    print "*" . nano_ref ($typedef, 'name') . "*::\n";
    print nano_ref ($typedef, 'doc') . "\n";
    print "+\n";
    
    my $params = nano_assoc ($typedef, 'params');
    foreach (@{$params}) {
        my $pt = nano_ref ($_, 'type');
        my $pd = nano_ref ($_, 'doc');
        print car ($_);
        if ($pt) {
            print " (" . type_short_desc ($pt) . ")";
        }
        if ($pd) {
            print ": " . $pd;
        }
        print "\n+\n";
    }

    my $base = nano_ref ($typedef, 'base');
    if ($base) {
        print "Base: " . type_short_desc ($base) . "\n";
    }
}

my $document = XML::DOM::Parser->new()->parse(STDIN);

foreach ($document->getDocumentElement()->getChildNodes()) {
    my $node = $_;
    if ($node->getNodeName() eq "doc") {
        output_doc ($node);
    } elsif ($node->getNodeName() eq "key") {
        output_key ($node);
    } elsif ($node->getNodeName() eq "type") {
        output_typedef ($node);
    }
}
