#!

i=0

rm -f hashes.txt

cat input.txt | while read line ; do
  UNAME=${line%:*}
  PW=${line#*:}
  echo ${UNAME}
  echo ${PW}
  hash="descrypt"
  if [ $i -gt 3 ] ; then
    hash="md5crypt"
  fi
  if [ $i -gt 8 ] ; then
    hash="sha512crypt"
  fi
  pwhash="$(echo ${PW} | mkpasswd -m ${hash} -s)"
  echo "${UNAME}:${pwhash}" >> hashes.txt
  i=$(($i+1))
done


