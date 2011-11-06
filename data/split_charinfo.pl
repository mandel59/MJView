#/usr/bin/env perl
use strict;
use warnings;
use utf8;
use Encode;
use DBI;

my $db_path = $ARGV[0] or die "no argument";
my $db_src = "dbi:SQLite:dbname=$db_path";
my $db = DBI->connect($db_src) or die "failed to connect to $db_path";

$db->do('begin');

$db->do("create table mj_info ( mj TEXT, svg_version TEXT, hanyodenshi INTEGER, heiseimincho TEXT, strokes INTEGER, history TEXT )");
$db->do("create table mj_index ( mj TEXT, radical INTEGER, strokes INTEGER )");
$db->do("create table mj_reading ( mj TEXT, reading TEXT )");
$db->do("create table mj_measure ( mj TEXT, measure TEXT )");
$db->do("create table mj_koseki ( mj TEXT, code INTEGER )");
$db->do("create table mj_juki ( mj TEXT, code INTEGER )");
$db->do("create table mj_toki ( mj TEXT, code INTEGER )");
$db->do("create table mj_x0213 ( mj TEXT, code TEXT, subsumption TEXT, class INTEGER )");
$db->do("create table mj_x0212 ( mj TEXT, code TEXT )");
$db->do("create table mj_ucs ( mj TEXT, ucs INTEGER, class TEXT, imp INTEGER )");
$db->do("create table mj_ivs ( mj TEXT, ucs INTEGER, ivs INTEGER, imp INTEGER )");
$db->do("create table mj_daikanwa ( mj TEXT, daikanwa TEXT )");

my %insert;
$insert{'info'} = $db->prepare("insert into mj_info values (?, ?, ?, ?, ?, ?)");
$insert{'index'} = $db->prepare("insert into mj_index values (?, ?, ?)");
$insert{'reading'} = $db->prepare("insert into mj_reading values (?, ?)");
$insert{'measure'} = $db->prepare("insert into mj_measure values (?, ?)");
$insert{'koseki'} = $db->prepare("insert into mj_koseki values (?, ?)");
$insert{'juki'} = $db->prepare("insert into mj_juki values (?, ?)");
$insert{'toki'} = $db->prepare("insert into mj_toki values (?, ?)");
$insert{'x0213'} = $db->prepare("insert into mj_x0213 values (?, ?, ?, ?)");
$insert{'x0212'} = $db->prepare("insert into mj_x0212 values (?, ?)");
$insert{'ucs'} = $db->prepare("insert into mj_ucs values (?, ?, ?, ?)");
$insert{'ivs'} = $db->prepare("insert into mj_ivs values (?, ?, ?, ?)");
$insert{'daikanwa'} = $db->prepare("insert into mj_daikanwa values (?, ?)");

<STDIN>;
while( <STDIN> ) {
    my @row = split(",",decode("Shift_JIS", $_));
    my $mj_glyph = $row[0];
    my $svg_version = $row[1];
    my $hanyodenshi = $row[2];
    my $heiseimincho = $row[3];
    my @index = ( [$row[4], $row[5]], [$row[6], $row[7]], [$row[8], $row[9]], [$row[10], $row[11]] );
    my $strokes = $row[12];
    my @readings = split("・", $row[13]);
    my $measure = $row[14];
    my ($koseki, $juki, $toki) = ($row[15], $row[16], $row[17]);
    my ($x0213, $x0213_subsumption, $x0213_class) = ($row[18], $row[19], $row[20]);
    my $x0212 = $row[21];
    my ($ucs, $ucs_class, $ucs_imp) = ($row[22], $row[23], $row[24]);
    my ($ivs_ucs, $ivs) = split("_", $row[25]);
    my $ivs_imp = $row[26];
    my $daikanwa = $row[27];
    my $history = $row[28];
    $history =~ s/[\r\n]//g;
    $insert{'info'}->execute($mj_glyph, $svg_version, $hanyodenshi, $heiseimincho, $strokes, $history);
    foreach my $i (@index) {
        $insert{'index'}->execute($mj_glyph, $i->[0], $i->[1]) if $i->[0];
    }
    foreach my $reading (@readings) {
        $insert{'reading'}->execute($mj_glyph, $reading);
    }
    $insert{'measure'}->execute($mj_glyph, $measure) if $measure;
    $insert{'koseki'}->execute($mj_glyph, $koseki) if $koseki;
    $insert{'juki'}->execute($mj_glyph, hex(substr($juki, 2))) if $juki;
    $insert{'toki'}->execute($mj_glyph, $toki) if $toki;
    $insert{'x0213'}->execute($mj_glyph, $x0213, $x0213_subsumption, $x0213_class) if $x0213;
    $insert{'x0212'}->execute($mj_glyph, $x0212) if $x0212;
    $insert{'ucs'}->execute($mj_glyph, hex(substr($ucs, 2)), $ucs_class, $ucs_imp?1:0) if $ucs;
    $insert{'ivs'}->execute($mj_glyph, hex($ivs_ucs), hex($ivs), $ivs_imp?1:0) if $ivs;
    $insert{'daikanwa'}->execute($mj_glyph, $daikanwa) if $daikanwa;
}

$db->do('commit');

$db->disconnect;

