#!/usr/bin/perl

use strict;
use warnings;

my @TSTATS = (0) x 256;
my %NGRAPH_STATS;
my %WORD_STATS;
my $CTEXT;
my $KEYLEN;
my $WORDLEN = 8; # Don't decrease in size without fixing n-graph prints
my @keyslot_stats;
my $BRUTE_DEPTH = 4;

my $max_guesses = 1000;
my $gnum = 0;
my %guesses;

# Basic alpha shifting
#my $keymin = ord('A');
#my $keymax = ord('Z');

# Basic byte ops
my $keymin = 0;
my $keymax = 255;

# Affine alpha transforms (needs about 75 chars of ctext per keybyte)
#my $keymin = 0;
#my $keymax = (12 * 26) - 1;
my @AFFINE_ALPHA = (1, 3, 5, 7, 9, 11, 15, 17, 19, 21, 23, 25);

my %fact_cache;
my $max_keylen = 20;


unless ((defined $ARGV[0]) &&
	(-e $ARGV[0])) {
    die 'You must specify a training file as the first argument!', "\n";
}

unless ((defined $ARGV[1]) &&
	(-e $ARGV[1])) {
    die 'You must specify a ciphertext file as the second argument!', "\n";
}

unless ((defined $ARGV[2]) &&
	($ARGV[2] =~ m/^\d+$/)) {
    die 'You must specify a key length guess as the third argument!', "\n";
}
$KEYLEN = $ARGV[2];


my $train;
open (IN, $ARGV[0]) or die 'Unable to open training file: ', $!, "\n";
{
    local $/ = undef;
    $train  = <IN>;
}
close IN;




# Do the letter freq stats
my $len = length $train;
my $inc = (1.0 / $len);
for (my $i = 0; $i < $len; $i++) {
    $TSTATS[ord(substr($train, $i, 1))] += $inc;
}
#print 'stats: ', join(', ', @train_stats), "\n";

# Do the word stats
for (my $i = 0; $i < ($len - $WORDLEN); $i++) {
    my $word = substr($train, $i, $WORDLEN);

    unless (exists $WORD_STATS{$word}) {
	$WORD_STATS{$word} = 0;
    }

    $WORD_STATS{$word} += $WORDLEN;
}


open (IN, $ARGV[1]) or die 'Unable to open ciphertext file: ', $!, "\n";
{
    local $/ = undef;
    $CTEXT  = <IN>;
}
close IN;

# TRUNCATE CTEXT TO 4K
$CTEXT = substr($CTEXT, 0, 4096);


# Do the n-graph stats
my $clen = length $CTEXT;
for (my $n = 2; $n <= $WORDLEN; $n++) {
    warn 'counting n-graph stats for n= ', $n, "\n";

    $NGRAPH_STATS{$n} = {};
    for (my $i = 0; $i < ($clen - $n); $i++) {
	my $graph = substr($CTEXT, $i, $n);

	unless (exists $NGRAPH_STATS{$n}{$graph}) {
	    $NGRAPH_STATS{$n}{$graph} = ();
	}

	push @{$NGRAPH_STATS{$n}{$graph}}, $i;
    }
}


# Now find the factors in the n-graph differences
my %keylen_votes;
for (my $n = 2; $n <= $WORDLEN; $n++) {
    warn 'Diffing n-graph indexes for n= ', $n, "\n";

    $keylen_votes{$n} = {};
    foreach my $graph (keys %{$NGRAPH_STATS{$n}}) {

	my $ilen = scalar @{$NGRAPH_STATS{$n}{$graph}};
	for (my $i = 0; $i < ($ilen - 1); $i++) {

	    my $diff = ($NGRAPH_STATS{$n}{$graph}[$i + 1] -
			$NGRAPH_STATS{$n}{$graph}[$i]);

	    next if ($diff < $n);

	    my @votes = get_factors($diff);

	    foreach my $vote (@votes) {
		unless (exists $keylen_votes{$n}{$vote}) {
		    $keylen_votes{$n}{$vote} = 0;
		}

		$keylen_votes{$n}{$vote} += 1;
	    }
	}
    }
}


# Now report on the keylen votes:
print '-' x 79, "\n";
for (my $n = $WORDLEN; $n >= 2; $n--) {

    my @vote_list;
    foreach my $vote (sort {my $c = ($keylen_votes{$n}{$b} <=> $keylen_votes{$n}{$a});
			    if ($c == 0) {
				return ($b <=> $a);
			    }
			    else {
				return $c;
			    }
		      } keys %{$keylen_votes{$n}}) {
	push @vote_list, $vote . ':' . $keylen_votes{$n}{$vote};
    }

    print $n, '-graph key length votes: ', join(', ', @vote_list), "\n";
}

print '-' x 79, "\n";




# Yes I know this is terribly slow...
sub get_factors {
    my $int = shift;

    if ($int == 1) {
	return (1);
    }

    if (exists $fact_cache{$int}) {
	return @{$fact_cache{$int}}
    }

    my @facts = ();
    for (my $i = 1; ($i <= $int) && ($i <= $max_keylen); $i++) {

	if ($int % $i == 0) {
	    push @facts, $i;
	}
    }

    $fact_cache{$int} = [@facts];

    return @facts;
}


sub do_sub {
    my $cbyte = shift;
    my $kval = shift;

    # [A-Z] add kval (this is encrypting viginere)
    #return chr((((ord($cbyte) - ord('A')) +
    #($kval - ord('A'))) % 26) + ord('A'));

    # [A-Z] sub kval (this is decrypting viginere)
    #return chr(((((ord($cbyte) - ord('A')) -
    # ($kval - ord('A'))) + 26) % 26) + ord('A'));

    # XOR kval
    return chr(ord($cbyte) ^ $kval);

    # Byte addition
    #return chr((ord($cbyte) + $kval) & 0xFF);

    # Affine transform
    #return chr(((($AFFINE_ALPHA[int($kval / 26)] *
	#	  (ord($cbyte) - ord('A'))) +
	#	 ($kval % 26)) % 26) + ord('A'));

    # Affine transform (inverse)
    #return chr((($AFFINE_ALPHA[int($kval / 26)] *
	#	 ((ord($cbyte) - ord('A')) -
	#	  ($kval % 26))) % 26) + ord('A'));
}


sub apply_key {
    my @key = @_;

    my $len = length $CTEXT;
    my @plist;
    for (my $i = 0; $i < $len; $i++) {
	push @plist, do_sub(substr($CTEXT, $i, 1),
			    $key[$i % $KEYLEN]);
    }

    return join('', @plist);
}


sub get_score {
    my @ptext = @_;

    my $len = scalar @ptext;
    my @pstats = (0) x 256;
    my $inc = (1.0 / $len);
    for (my $i = 0; $i < $len; $i++) {
	$pstats[ord($ptext[$i])] += $inc;
    }

    my $score = 0;
    for (my $i = 0; $i < 256; $i++) {
	$score += ($pstats[$i] - $TSTATS[$i]) ** 2;
    }

    return $score;
}


sub get_word_score {
    my @ptext = @_;

    my $gplain = join('', @ptext);
    my $len = length $gplain;
    my $score = 0;
    for (my $i = 0; $i < ($len - $WORDLEN); $i++) {
	my $word = substr($gplain, $i, $WORDLEN);

	if (exists $WORD_STATS{$word}) {
	    $score += $WORD_STATS{$word};
	}
    }

    return $score;
}


sub clean_text {
    my $ptext = shift;

    $ptext =~ s/[^!-~]/./smg;

    return $ptext;
}


sub clean_chr {
    my $c = shift;

    if (($c < ord('!')) ||
	($c > ord('~'))) {
	return $c;
    }
    else {
	return '"' . chr($c) . '"';
    }
}


sub semi_opt_key {
    my ($slot, $keyref, $last) = @_;

    #print 'max: ', $max_guesses, "\n";
    #print 'gnum: ', $gnum, "\n";
    return if ($gnum >= $max_guesses);

    if ($slot == $KEYLEN) {
	my $ptext = apply_key(@{$keyref});

    my $keytext = pack('C*', @{$keyref});
    $keytext =~ s/[^!-~]/./g;
	$guesses{$gnum} = {
	    'key'=>'[' . join(', ', @{$keyref}) . ']',
        'keytext'=>'(' . $keytext . ')',
	    'sscore'=>get_score(split(//, $ptext)),
	    'wscore'=>get_word_score(split(//, $ptext)),
	    'ptext'=>substr(clean_text($ptext), 0, 256)
	};


	$gnum++;
	return;
    }

    my @kv_list = (sort {$keyslot_stats[$slot]{$a} <=>
			     $keyslot_stats[$slot]{$b}}
		   keys %{$keyslot_stats[$slot]});

    my $did_one = 0;
    foreach my $kv (@kv_list[0 .. $BRUTE_DEPTH]) {

	my $curscore = $keyslot_stats[$slot]{$kv};

	#if (($did_one == 0) ||
	 #   ($curscore <= $last)) {

	    my @nkey = @{$keyref};
	    push @nkey, $kv;
	    semi_opt_key($slot + 1, \@nkey, $curscore);

	    $did_one = 1;
	#}
	#else {
	#    return;
	#}
    }

}


print 'Score for ctext: ', get_score(split(//, $CTEXT)), "\n";

#print 'A + A: ', do_sub('A', 'A'), "\n";
#print 'A + B: ', do_sub('A', 'B'), "\n";
#print 'B + B: ', do_sub('B', 'B'), "\n";
#print 'B + Z: ', do_sub('B', 'Z'), "\n";

# Do the stats on a key-byte basis


# We'll loop for each byte in the key
for (my $slot = 0; $slot < $KEYLEN; $slot++) {
    push @keyslot_stats, {};

    # Now for each possible keyval
    foreach my $kv ($keymin .. $keymax) {

	my @plist;
	for (my $i = $slot; $i < $clen; $i += $KEYLEN) {
	    push @plist, do_sub(substr($CTEXT, $i, 1), $kv);
	}

	$keyslot_stats[$slot]{$kv} = get_score(@plist);

	#print 'Got score for ', $slot, '/', $kb, ' : ',
	#$keyslot_stats[$slot]{ord($kb)}, "\n";
    }
}


for (my $slot = 0; $slot < $KEYLEN; $slot++) {

    print 'Top ', $BRUTE_DEPTH, ' keys for slot ', $slot, ': ',
    join(', ', (sort {$keyslot_stats[$slot]{$a} <=>
			  $keyslot_stats[$slot]{$b}}
		keys %{$keyslot_stats[$slot]})[0 .. ($BRUTE_DEPTH - 1)]), "\n";
}

#my $ptext = apply_key("GMARK");
#print $ptext, "\n";

# Try the possible keys in semi-optimal order
semi_opt_key(0, [], 2);

print "\n";
print '=' x 79, "\n";
print 'FIRST 5 GUESSES IN SEMI-OPTIMAL ORDER', "\n";
print '=' x 79, "\n";
foreach my $guess_num ((sort {$a <=> $b} keys %guesses)[0 .. 4]) {

    print 'guess key # ', $guess_num, ' : ',
    $guesses{$guess_num}{'key'}, ' ', $guesses{$guess_num}{'keytext'}, ' score: ',
    $guesses{$guess_num}{'sscore'}, ' word score: ',
    $guesses{$guess_num}{'wscore'}, ' : ', "\n",
    $guesses{$guess_num}{'ptext'}, "\n\n";

}


print "\n";
print '=' x 79, "\n";
print 'FIRST 5 GUESSES IN STATISTICAL SCORE ORDER', "\n";
print '=' x 79, "\n";
foreach my $guess_num ((sort {$guesses{$a}{'sscore'} <=>
                                  $guesses{$b}{'sscore'}}
                        keys %guesses)[0 .. 4]) {

    print 'guess key # ', $guess_num, ' : ',
    $guesses{$guess_num}{'key'}, ' ', $guesses{$guess_num}{'keytext'}, ' score: ',
    $guesses{$guess_num}{'sscore'}, ' word score: ',
    $guesses{$guess_num}{'wscore'}, ' : ', "\n",
    $guesses{$guess_num}{'ptext'}, "\n\n";

}

print "\n";
print '=' x 79, "\n";
print 'FIRST 5 GUESSES IN WORD SCORE ORDER', "\n";
print '=' x 79, "\n";
foreach my $guess_num ((sort {$guesses{$b}{'wscore'} <=>
                                  $guesses{$a}{'wscore'}}
                        keys %guesses)[0 .. 4]) {

    print 'guess key # ', $guess_num, ' : ',
    $guesses{$guess_num}{'key'}, ' ', $guesses{$guess_num}{'keytext'}, ' score: ',
    $guesses{$guess_num}{'sscore'}, ' word score: ',
    $guesses{$guess_num}{'wscore'}, ' : ', "\n",
    $guesses{$guess_num}{'ptext'}, "\n\n";

}
