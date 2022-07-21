#!/usr/bin/perl
#  genWords.pl
#  wordlesolve
#
#  Created by Jeremy Rand on 2022-07-21.
#  Copyright © 2022 Jeremy Rand. All rights reserved.

use strict;

die "$0: Expected two arguments, input file and output file" if ($#ARGV != 1);

my $gInputFileName = $ARGV[0];
my $gGenDir = $ARGV[1];
my $gOutputFileName = $gGenDir . "/wordData.s";

open my $gOutputFile, ">", "$gOutputFileName" or die "$0: Unable to open $gOutputFileName for writing, $!";
open my $gInputFile, "<", "$gInputFileName" or die "$0: Unable to open $gInputFileName for reading, $!";

my $text = << "EOF";
    case on
    mcopy wordData.macros
    keep wordData

wordData data wordDataSeg
EOF
print $gOutputFile $text;

my @words;
while (<$gInputFile>) {
    chomp;
    tr/[a-z]/[A-Z]/;
    push(@words, "    dc c'$_'\n");
}
close($gInputFile);

print $gOutputFile "    dc i2'" . ($#words + 1) ."'\n";
print $gOutputFile join("", @words);

$text = << "EOF";
    end
EOF
print $gOutputFile $text;
close($gOutputFile);
exit(0);
