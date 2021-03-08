#!/usr/bin/perl

use strict;
use warnings;

my @gens = (
    [(63, 62, 40, 42, 34, 32, 61, 22, 27, 28, 23, 10, 29, 45, 5, 19, 16, 24, 46, 33, 17, 14, 8, 47, 18, 44, 25, 26, 53, 38, 21, 52, 15, 1, 31, 0, 59, 36, 56, 6, 37, 41, 58, 51, 54, 11, 2, 50, 55, 4, 12, 39, 35, 13, 43, 3, 30, 7, 9, 48, 57, 60, 20, 49)], # 0
    [(0, 39, 59, 31, 9, 41, 52, 1, 25, 55, 15, 37, 28, 48, 43, 58, 40, 38, 24, 18, 10, 60, 44, 63, 20, 57, 26, 27, 45, 4, 54, 46, 51, 42, 47, 29, 6, 11, 16, 35, 32, 49, 5, 53, 30, 8, 56, 19, 50, 2, 23, 34, 14, 3, 12, 22, 13, 17, 21, 33, 62, 61, 36, 7)], # 1
    [(10, 12, 52, 42, 7, 36, 13, 57, 61, 44, 39, 22, 32, 17, 8, 59, 27, 29, 2, 38, 46, 21, 0, 4, 5, 6, 62, 58, 9, 48, 45, 16, 1, 35, 3, 31, 23, 54, 56, 41, 49, 24, 33, 40, 50, 63, 43, 25, 37, 11, 53, 55, 15, 26, 60, 20, 18, 34, 47, 30, 14, 51, 28, 19)], # 2
    [(6, 58, 45, 55, 14, 37, 32, 8, 4, 53, 13, 16, 47, 0, 23, 19, 49, 10, 30, 52, 3, 21, 27, 60, 48, 12, 9, 11, 50, 39, 56, 40, 25, 46, 51, 43, 54, 24, 15, 17, 35, 29, 20, 33, 26, 18, 42, 1, 41, 31, 62, 57, 63, 28, 5, 34, 59, 61, 22, 38, 36, 7, 44, 2)], # 3
    [(34, 60, 50, 25, 9, 40, 21, 26, 29, 59, 1, 44, 42, 15, 23, 48, 57, 27, 55, 6, 11, 33, 35, 38, 51, 16, 53, 14, 52, 47, 49, 5, 54, 4, 12, 18, 61, 24, 3, 43, 0, 36, 45, 17, 62, 63, 28, 58, 37, 13, 8, 31, 56, 19, 10, 39, 30, 41, 20, 22, 32, 2, 7, 46)], # 4
    [(61, 32, 35, 18, 48, 47, 11, 19, 3, 44, 39, 21, 52, 43, 50, 1, 51, 4, 34, 16, 7, 54, 36, 2, 22, 9, 28, 20, 62, 14, 31, 55, 38, 13, 42, 58, 41, 33, 15, 0, 27, 23, 53, 57, 6, 30, 60, 29, 46, 26, 45, 56, 8, 59, 25, 37, 63, 10, 49, 17, 5, 40, 12, 24)], # 5
    [(61, 57, 0, 32, 37, 30, 7, 28, 14, 17, 21, 27, 18, 58, 40, 33, 54, 13, 2, 42, 35, 44, 53, 29, 55, 8, 20, 59, 48, 52, 51, 16, 46, 11, 39, 24, 36, 4, 41, 31, 60, 43, 62, 10, 6, 34, 56, 47, 38, 45, 23, 63, 26, 49, 1, 3, 12, 19, 5, 25, 15, 22, 9, 50)], # 6
    [(18, 41, 21, 26, 10, 13, 32, 51, 33, 16, 54, 28, 25, 45, 11, 39, 56, 37, 42, 2, 57, 31, 4, 9, 24, 43, 27, 59, 29, 6, 17, 20, 44, 23, 22, 34, 53, 35, 52, 19, 0, 63, 30, 61, 7, 38, 49, 48, 60, 1, 46, 5, 55, 50, 8, 3, 40, 58, 36, 15, 62, 47, 14, 12)], # 7
    [(43, 28, 21, 62, 39, 35, 33, 30, 63, 57, 26, 8, 19, 32, 23, 6, 16, 38, 5, 60, 53, 56, 37, 3, 48, 11, 18, 34, 22, 25, 0, 46, 47, 52, 51, 59, 42, 9, 24, 31, 50, 61, 2, 20, 15, 29, 17, 1, 49, 40, 55, 13, 4, 12, 10, 41, 14, 58, 54, 7, 36, 27, 44, 45)], # 8
    [(21, 9, 57, 35, 19, 41, 3, 8, 30, 47, 42, 23, 52, 18, 15, 59, 11, 37, 54, 25, 53, 51, 56, 44, 16, 46, 26, 12, 43, 2, 28, 14, 38, 29, 10, 61, 1, 45, 7, 39, 13, 6, 27, 5, 60, 22, 17, 4, 49, 55, 36, 0, 62, 63, 31, 32, 24, 48, 40, 34, 33, 20, 58, 50)], # 9
    [(43, 45, 51, 26, 10, 9, 18, 55, 3, 33, 38, 59, 32, 35, 0, 41, 15, 25, 60, 44, 57, 61, 52, 4, 24, 47, 54, 58, 34, 36, 30, 12, 42, 53, 46, 8, 20, 14, 6, 39, 2, 7, 27, 21, 29, 50, 48, 31, 40, 11, 49, 17, 1, 28, 22, 23, 19, 5, 56, 62, 13, 16, 63, 37)], # A
    [(12, 9, 28, 13, 29, 63, 10, 39, 48, 15, 32, 0, 19, 1, 34, 25, 46, 50, 45, 2, 22, 33, 26, 7, 58, 20, 27, 11, 18, 21, 59, 62, 14, 52, 40, 8, 23, 57, 38, 41, 35, 5, 4, 49, 31, 60, 37, 16, 61, 51, 6, 56, 36, 43, 54, 55, 24, 17, 3, 42, 53, 44, 30, 47)], # B
    [(29, 62, 32, 17, 52, 13, 34, 46, 48, 51, 54, 20, 3, 63, 41, 56, 19, 44, 16, 58, 6, 37, 59, 0, 28, 33, 53, 47, 21, 12, 9, 25, 40, 26, 4, 1, 61, 42, 60, 18, 10, 31, 2, 7, 49, 23, 45, 15, 11, 30, 39, 8, 50, 55, 24, 14, 43, 27, 36, 57, 38, 35, 5, 22)], # C
    [(52, 31, 10, 18, 59, 33, 63, 44, 1, 61, 28, 5, 39, 26, 34, 12, 56, 16, 15, 43, 13, 2, 55, 9, 37, 8, 11, 0, 42, 50, 36, 51, 54, 48, 22, 23, 46, 32, 60, 47, 24, 4, 40, 17, 19, 30, 49, 29, 62, 58, 27, 35, 57, 20, 21, 6, 3, 41, 7, 14, 38, 45, 25, 53)], # D
    [(48, 49, 22, 56, 5, 43, 25, 9, 37, 19, 28, 55, 54, 59, 31, 46, 40, 11, 30, 8, 4, 61, 51, 18, 24, 57, 15, 17, 44, 58, 42, 3, 47, 60, 27, 39, 33, 20, 62, 35, 45, 23, 36, 2, 13, 10, 1, 0, 6, 38, 53, 14, 63, 16, 29, 52, 7, 12, 34, 26, 50, 21, 41, 32)], # E
    [(26, 14, 56, 29, 51, 61, 15, 54, 2, 63, 30, 40, 62, 27, 52, 4, 0, 21, 13, 38, 33, 53, 5, 8, 39, 36, 47, 57, 58, 43, 7, 60, 1, 9, 25, 28, 24, 55, 23, 41, 45, 49, 12, 10, 22, 35, 11, 6, 50, 16, 32, 31, 44, 37, 48, 42, 46, 59, 34, 3, 18, 20, 17, 19)] # F
    );

my @hlet = ('0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F');

my $N = 64;

my $o; # The state rotation offset
my @state = (46, 35, 0, 49, 17, 55, 26, 28, 38, 24, 18, 57, 12, 9, 34, 2, 61, 54, 4, 41, 53, 10, 59, 3, 50, 21, 60, 20, 45, 33, 22, 42, 19, 11, 30, 40, 44, 51, 32, 13, 29, 7, 14, 58, 63, 27, 48, 36, 8, 31, 6, 37, 23, 56, 47, 62, 1, 39, 15, 25, 43, 5, 16, 52);
#my @state = (0 .. ($N - 1));

#apply_perms_from_str(\@state, 'DCDD');
#print 'state: ', join(', ', @state), "\n";
#exit 0;

#my @state = qw/44 28 43 25 54 2 14 21 20 47 49 36 41 27 57 26 12 15 34 42 37 63 19 7 30 11 23 31 51 24 59 3 32 22 29 18 62 40 0 9 55 13 1 48 50 58 10 45 35 4 60 39 38 46 17 52 53 16 33 5 56 61 6 8/;

#my @goal = (0 .. ($N - 1));
my @goal = (24, 10, 20, 14, 53, 1, 42, 17, 8, 44, 27, 9, 0, 28, 22, 12, 13, 52, 23, 15, 48, 37, 58, 25, 40, 30, 49, 36, 6, 34, 2, 57, 7, 56, 35, 18, 19, 45, 16, 31, 50, 59, 32, 61, 26, 51, 38, 41, 29, 39, 47, 5, 46, 62, 63, 43, 33, 60, 54, 21, 55, 3, 11, 4);
my @gidx = (0 x $N);

# Everything is going to be done relative to 0 so find where it is and call that 'solved' but at a different offset
for (my $i = 0; $i < $N; $i++) {
    $gidx[$goal[$i]] = $i;
}

for (my $i = 0; $i < $N; $i++) {
    if ($state[$i] == $goal[0]) {
        $o = $i;
        last;
    }
}
#print 'set initial offset to ', $o, "\n";


#print 'goal: ';
#print_state(\@goal);

#print 'goal index: ';
#print_state(\@gidx);

my $m = solve('', $o, \@state, \@goal, 1, 0);

print 'Length ', (length($m) / 2), ' solution found: ', "\n";
print $m, "\n";


sub is_solved {
    my $sref = shift;

    for (my $i = 0; $i < $N; $i++) {
        if ($sref->[$i] != $goal[$i]) {
            return 0;
        }
    }

    return 1;
}


# func (s *Permhash296) apply_perm (perm []int) {

#     var tmp [N]int

#     for i := 0; i < N; i++ {
#     tmp[i] = s.State[perm[i]]
#     }

#     copy(s.State[:], tmp[:])
# }

sub apply_perm {
    my $sref = shift;
    my $pref = shift;

    my @tmp = (0 .. ($N - 1));

    for (my $i = 0; $i < $N; $i++) {
        $tmp[$i] = $sref->[$pref->[$i]];
    }

    for (my $i = 0; $i < $N; $i++) {
        $sref->[$i] = $tmp[$i];
    }
}


sub apply_perms_from_str {
    my $sref = shift;
    my $str = shift;

    foreach my $l (split(//, $str)) {
        apply_perm($sref, $gens[hex($l)]);
    }
}



sub rot {
    my $sref = shift;

    unshift @$sref, pop @$sref;


    #print 'rotating', "\n";
    return '011';
}


sub swap_0_1 {
    my $sref = shift;

    ($sref->[0], $sref->[1]) = ($sref->[1], $sref->[0]);

    #print 'swapping', "\n";
    return '233';
}

sub swap_6_58 {
    my $sref = shift;

    ($sref->[6], $sref->[58]) = ($sref->[58], $sref->[6]);

    #print 'swapping', "\n";
    return '323';
}


sub swap_22_32 {
    my $sref = shift;

    ($sref->[22], $sref->[32]) = ($sref->[32], $sref->[22]);

    #print 'swapping', "\n";
    return '332';
}


sub swap_51_52 {
    my $sref = shift;

    ($sref->[51], $sref->[52]) = ($sref->[52], $sref->[51]);

    #print 'swapping', "\n";
    return 'DCDD';
}


sub swap_35_57 {
    my $sref = shift;

    ($sref->[35], $sref->[57]) = ($sref->[57], $sref->[35]);

    #print 'swapping', "\n";
    return 'DDCD';
}


sub swap_23_41 {
    my $sref = shift;

    ($sref->[23], $sref->[41]) = ($sref->[41], $sref->[23]);

    #print 'swapping', "\n";
    return 'DDDC';
}


sub print_state {
    my $sref = shift;

    print '[', join(',', @{$sref}), ']', "\n";
}

sub rel_idx {
    my $i = shift;

    return (($N - $o) + $i) % $N;
}


sub compress_rots {
    my $m = shift;

    my $len_m;
    do {
        $len_m = length($m);

        # 64 rots go away
        $m =~ s/011011011011011011011011011011011011011011011011011011011011011011011011011011011011011011011011011011011011011011011011011011011011011011011011011011011011011011011011011011011011011011011011//g;
        $m =~ s/ABAB//g;
        $m =~ s/8989898989898989//g;

        # 32 rots -> AB
        $m =~ s/011011011011011011011011011011011011011011011011011011011011011011011011011011011011011011011011/AB/g;
        $m =~ s/89898989/AB/g;

        # 8 rots -> 89
        $m =~ s/011011011011011011011011/89/g;

        # All repeated swaps go away
        $m =~ s/233233//g;
        $m =~ s/323323//g;
        $m =~ s/332332//g;
        $m =~ s/CDDDCDDD//g;
        $m =~ s/DCDDDCDD//g;
        $m =~ s/DDCDDDCD//g;
        $m =~ s/DDDCDDDC//g;

    } while (length($m) < $len_m);

    return $m;
}


sub solve {
    my $m = shift;
    my $o = shift;
    my $sref = shift;
    my $gref = shift;
    my $brute = shift;
    my $inbrute = shift;

    my @state = @{$sref};
    my @goal = @{$gref};

    #print_state(\@state);


    while (is_solved(\@state) != 1) {

        if ($brute > 0) {
            # measure how we'd do first
            my $sm = solve($m, $o, \@state, \@goal, $brute - 1, 1);

            my $bsm = $sm;       # best sm
            my $bm =   $m;       # best sm
            my @bstate = @state; # best state
            my $bo = $o;         # best o

            my $better = 0;

            my @bstrs = ();

            # known useful sequences:

            if ($inbrute == 0) {
                # Known rotations and their combos
                push @bstrs, '011';
                push @bstrs, '011' x 2;
                push @bstrs, '011' x 3;
                push @bstrs, '011' x 4;
                push @bstrs, '011' x 5;
                push @bstrs, '011' x 6;
                push @bstrs, '011' x 7;

                push @bstrs, '89';
                push @bstrs, '89' . '011';
                push @bstrs, '89' . ('011' x 2);
                push @bstrs, '89' . ('011' x 3);
                push @bstrs, '89' . ('011' x 4);
                push @bstrs, '89' . ('011' x 5);
                push @bstrs, '89' . ('011' x 6);
                push @bstrs, '89' . ('011' x 7);

                push @bstrs, '8989';
                push @bstrs, '8989' . '011';
                push @bstrs, '8989' . ('011' x 2);
                push @bstrs, '8989' . ('011' x 3);
                push @bstrs, '8989' . ('011' x 4);
                push @bstrs, '8989' . ('011' x 5);
                push @bstrs, '8989' . ('011' x 6);
                push @bstrs, '8989' . ('011' x 7);

                push @bstrs, '898989';
                push @bstrs, '898989' . '011';
                push @bstrs, '898989' . ('011' x 2);
                push @bstrs, '898989' . ('011' x 3);
                push @bstrs, '898989' . ('011' x 4);
                push @bstrs, '898989' . ('011' x 5);
                push @bstrs, '898989' . ('011' x 6);
                push @bstrs, '898989' . ('011' x 7);

                push @bstrs, 'AB';
                push @bstrs, 'AB' . '011';
                push @bstrs, 'AB' . ('011' x 2);
                push @bstrs, 'AB' . ('011' x 3);
                push @bstrs, 'AB' . ('011' x 4);
                push @bstrs, 'AB' . ('011' x 5);
                push @bstrs, 'AB' . ('011' x 6);
                push @bstrs, 'AB' . ('011' x 7);

                push @bstrs, 'AB89';
                push @bstrs, 'AB89' . '011';
                push @bstrs, 'AB89' . ('011' x 2);
                push @bstrs, 'AB89' . ('011' x 3);
                push @bstrs, 'AB89' . ('011' x 4);
                push @bstrs, 'AB89' . ('011' x 5);
                push @bstrs, 'AB89' . ('011' x 6);
                push @bstrs, 'AB89' . ('011' x 7);

                push @bstrs, 'AB8989';
                push @bstrs, 'AB8989' . '011';
                push @bstrs, 'AB8989' . ('011' x 2);
                push @bstrs, 'AB8989' . ('011' x 3);
                push @bstrs, 'AB8989' . ('011' x 4);
                push @bstrs, 'AB8989' . ('011' x 5);
                push @bstrs, 'AB8989' . ('011' x 6);
                push @bstrs, 'AB8989' . ('011' x 7);

                push @bstrs, 'AB898989';
                push @bstrs, 'AB898989' . '011';
                push @bstrs, 'AB898989' . ('011' x 2);
                push @bstrs, 'AB898989' . ('011' x 3);
                push @bstrs, 'AB898989' . ('011' x 4);
                push @bstrs, 'AB898989' . ('011' x 5);
                push @bstrs, 'AB898989' . ('011' x 6);
                push @bstrs, 'AB898989' . ('011' x 7);


                push @bstrs, 'EFF';
                push @bstrs, 'EFF' x 3;
                push @bstrs, 'EFF' x 4;
                push @bstrs, 'EFF' x 5;
                push @bstrs, 'EFF' x 6;
                push @bstrs, 'EFF' x 7;
            }


            # All swaps
            push @bstrs, '233';
            push @bstrs, '323';
            push @bstrs, '332';
            push @bstrs, 'CDDD';
            push @bstrs, 'DCDD';
            push @bstrs, 'DDCD';
            push @bstrs, 'DDDC';

            # More obscure rotation-like sequences:
            push @bstrs, '98';
            push @bstrs, '9898';
            push @bstrs, 'BA';
            push @bstrs, 'B89A';

            # Short low-order
            push @bstrs, 'BC';
            push @bstrs, 'CB';

            # All length 1 sequences
            for (my $p = 0; $p < 16; $p++) {
                push @bstrs, $hlet[$p];
            }

            # for (my $p = 0; $p < 16; $p++) {
            #     for (my $q = 0; $q < 16; $q++) {
            #         push @bstrs, $hlet[$p] . $hlet[$q];
            #     }
            # }

            # for (my $p = 0; $p < 16; $p++) {
            #     for (my $q = 0; $q < 16; $q++) {
            #         for (my $r = 0; $r < 16; $r++) {
            #             push @bstrs, $hlet[$p] . $hlet[$q] . $hlet[$r];
            #         }
            #     }
            # }

            foreach my $s (@bstrs) {

                my @nstate = @state;
                apply_perms_from_str(\@nstate, $s);

                my @nwstate = @nstate;

                my $no = 0;
                for (my $i = 0; $i < $N; $i++) {
                    if ($nstate[$i] == $goal[0]) {
                        $no = $i;
                        last;
                    }
                }

                my $nm = $m . $s;
                my $nsm = solve($nm, $no, \@nstate, \@goal, $brute - 1, 1);

                if (length($nsm) < length($bsm)) {

                    warn $s, ' is less by ', ((length($bsm) - length($nsm)) / 2), "\n";

                    $bsm = $nsm;
                    $bm = $nm;
                    $bo = $no;
                    @bstate = @nstate;

                    $better = 1;
                }
            }

            if ($better == 1) {
                $m = $bm;
                $o = $bo;
                @state = @bstate;
            }

            #$brute = 0;

        }

        if ($o > 1) {
            if ($gidx[$state[0]] > $gidx[$state[1]]) {

                $m .= swap_0_1(\@state);
                #print_state(\@state);
            }
        }

        # This swap has proven to be worse on average
        # if (($o < 6) || ($o > 58)) {
        #     if ($gidx[$state[6]] > $gidx[$state[58]]) {
        #         #print 'doing 6 28 swap', "\n";
        #         $m .= swap_6_58(\@state);
        #     }
        # }

        if (($o < 22) || ($o > 32)) {
            if ($gidx[$state[22]] > $gidx[$state[32]]) {
                #print 'doing 22 32 swap', "\n";
                $m .= swap_22_32(\@state);
            }
        }
        if (($o > 22) && ($o < 32)) {
            if ($gidx[$state[22]] < $gidx[$state[32]]) {
                #print 'doing 22 32 swap', "\n";
                $m .= swap_22_32(\@state);
            }
        }

        if (($o < 35) || ($o > 57)) {
            if ($gidx[$state[35]] > $gidx[$state[57]]) {
                #print 'doing 22 32 swap', "\n";
                $m .= swap_35_57(\@state);
            }
        }
        if (($o > 35) && ($o < 57)) {
            if ($gidx[$state[35]] < $gidx[$state[57]]) {
                #print 'doing 22 32 swap', "\n";
                $m .= swap_35_57(\@state);
            }
        }

        if (($o < 23) || ($o > 41)) {
            if ($gidx[$state[23]] > $gidx[$state[41]]) {
                #print 'doing 22 32 swap', "\n";
                $m .= swap_23_41(\@state);
            }
        }
        if (($o > 23) && ($o < 41)) {
            if ($gidx[$state[23]] < $gidx[$state[41]]) {
                #print 'doing 22 32 swap', "\n";
                $m .= swap_23_41(\@state);
            }
        }

        # This swap has proven to be worse on average
        # if (($o < 51) || ($o > 52)) {
        #     if ($gidx[$state[51]] > $gidx[$state[52]]) {
        #         #print 'doing 22 32 swap', "\n";
        #         $m .= swap_51_52(\@state);
        #     }
        # }


        #
        # if (($o > 6) && ($o < 58)) {
        #     if ($gidx[$state[6]] < $gidx[$state[58]]) {
        #         #print 'doing 6 28 swap', "\n";
        #         $m .= swap_6_58(\@state);
        #     }
        # }




        $m .= rot(\@state);
        $o = ($o + 1) % $N; # Rotate the state

        #print_state(\@state);
    }

    $m = compress_rots($m);

    if (length($m) % 2 == 1) {
        $m .= '456';
    }

    #print 'Length ', (length($m) / 2), ' solution found: ', "\n";
    #print $m, "\n";

    return $m;
}
