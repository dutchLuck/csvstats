# csvstats
Calculate the basic statistical descriptors for columns of numbers in a comma separated variable file. The stats are calculated using long double floating point precision and include sum, mean, median and standard deviation. The standard deviation is calculated from the semi-normalized column data, created by subtracting the mean from the data values to avoid numerical errors becoming large. The input file is assumed to have 1 or more columns of numbers, which do not have missing values, so that once the columns are counted in the first line of data, the rest of file is consistant with that count. Lines starting with a hash '#' are considered to be comment lines and are ignored, as are blank lines.

The usage information is; -
```
$ ./csvstats -h
Error: Please specify a filename to identify the csv file to read
Usage: csvstats [-ADhMNnPRSv] csvFileName
 where ;-
  The -A switch turns Off Arithmetic Mean (i.e. average ) output.
  The -D switch turns On Debug Information output.
  The -H switch turns On Header output mode.
  The -h switch turns On this output.
  The -M switch turns Off Median value output.
  The -N switch turns Off most Negetive value output.
  The -n switch turns Off the linefeed terminator on the output.
  The -P switch turns Off most Positive value output.
  The -p switch turns Off most Pearson's skew value output.
  The -R switch turns Off Range (i.e. most Pos - most Neg) value output.
  The -S switch turns Off Standard Deviation value output.
  The -v switch turns On verbose output (notably a header).
$
```
