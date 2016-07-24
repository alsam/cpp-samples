#!/usr/bin/perl

# -*- Perl -*-


#---------------------------------------------------------------------
#  Created:            Thursday, May 20, 2010
#  Original author:    Alexander Samoilov
#---------------------------------------------------------------------

#
#   Brief file description here
#

sub reflect_x  {
  my $str = "@_";
  my @str_arr = split //, $str;
  my $ret = "";
  foreach $char (@str_arr) {
    if    ($char eq 'U') { $ret .= 'D';  }
    elsif ($char eq 'D') { $ret .= 'U';  }
    else                 { $ret .= $char; }
  }
  return $ret;
}

sub reflect_y  {
  my $str = "@_";
  my @str_arr = split //, $str;
  my $ret = "";
  foreach $char (@str_arr) {
    if    ($char eq 'R') { $ret .= 'L';  }
    elsif ($char eq 'L') { $ret .= 'R';  }
    else                 { $ret .= $char; }
  }
  return $ret;
}

sub rotate_90_ccw  {
  my $str = "@_";
  my @str_arr = split //, $str;
  my $ret = "";
  foreach $char (@str_arr) {
    if    ($char eq 'R') { $ret .= 'D';  }
    elsif ($char eq 'U') { $ret .= 'R';  }
    elsif ($char eq 'L') { $ret .= 'U';  }
    else                 { $ret .= $char; }
  }
  return $ret;
}

sub replicate {
  my ($char, $scale_factor) = @_;
  my $ret = "";
  for (my $i = 0; $i < $scale_factor; $i++) {
    $ret .= $char;
  }
  return $ret;
}

sub scale {
  my ($str, $scale_factor) = @_;
  my @str_arr = split //, $str;
  my $ret = "";
  foreach $char (@str_arr) {
    $ret .= &replicate($char, $scale_factor);
  }
  return $ret;
}

sub translate_d {
  my ($y) = @_;
  my $ret = "P ";
  for (my $i = 0; $i < $y; $i++) {
    $ret .= 'D';
  }
  $ret .= " P";
  return $ret;
}

sub translate_u {
  my ($y) = @_;
  my $ret = "P ";
  for (my $i = 0; $i < $y; $i++) {
    $ret .= 'U';
  }
  $ret .= " P";
  return $ret;
}

sub translate_l {
  my ($x) = @_;
  my $ret = "P ";
  for (my $i = 0; $i < $x; $i++) {
    $ret .= 'L';
  }
  $ret .= " P";
  return $ret;
}

sub translate_r {
  my ($x) = @_;
  my $ret = "P ";
  for (my $i = 0; $i < $x; $i++) {
    $ret .= 'R';
  }
  $ret .= " P";
  return $ret;
}

sub translate_ru {
  my ($x, $y) = @_;
  my $ret = "P ";
  for (my $i = 0; $i < $x; $i++) {
    $ret .= 'R';
  }
  for (my $i = 0; $i < $y; $i++) {
    $ret .= 'U';
  }
  $ret .= " P";
  return $ret;
}


sub do_test {
  print &reflect_y("RULURULU");
  print "\n";

  print &scale("abc",7);
  print "\n";
  print &translate_ru("abc",7);
  print "\n";
}

#&do_test;

$basic = "RULURULU";

$basic_ccw_90 = &rotate_90_ccw($basic);

print &translate_ru(1,1);
print "\n$basic\n";

print &translate_d(4);
print &translate_ru(38,0);
print "\n" . &reflect_y($basic) . "\n";
print &translate_l(20);
print "\n$basic_ccw_90\n";

print &translate_l(10);
print &translate_ru(17,4);
print "\n" . &reflect_y($basic_ccw_90) . "\n";
print &translate_l(14);
print "\n$basic_ccw_90\n";

print &translate_l(8);
print &translate_u(4);
print "\n$basic_ccw_90\n";

print &translate_r(24);
print "\n" . &reflect_y($basic_ccw_90) . "\n";

print &translate_l(16);
print &translate_u(4);
print "\n" . &scale($basic_ccw_90, 4) . "\n";

print &translate_l(14);
print &translate_u(4);
print "\n" . &scale($basic_ccw_90, 3) . "\n";

print &translate_l(12);
print &translate_u(4);
print "\n" . &scale($basic_ccw_90, 2) . "\n";

print &translate_l(6);
print &translate_u(4);
print "\n$basic_ccw_90\n";


print "X";
