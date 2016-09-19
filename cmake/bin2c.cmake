# Convert any file to a C++ string
function(bin2c INFILE VARNAME OUTFILE)
  file(READ ${INFILE} HEXFILE HEX)
  string(LENGTH ${HEXFILE} XRSLEN)
  set(HEXPOS 0)
  file(WRITE ${OUTFILE}
          "/* generated from ${INFILE}; do not edit */\n"
          "#ifndef _${VARNAME}_H_\n"
          "#define _${VARNAME}_H_\n"
          "#include <string>\n"
          "const static std::string ${VARNAME} {")
  while (${HEXPOS} LESS ${XRSLEN})
    math(EXPR LPOS "${HEXPOS} % 32")
    if (NOT ${LPOS})
      file(APPEND ${OUTFILE} "\n")
    endif ()
    string(SUBSTRING ${HEXFILE} ${HEXPOS} 2 HEXBYTE)
    file(APPEND ${OUTFILE} "0x${HEXBYTE}")
    math(EXPR HEXPOS "${HEXPOS} + 2")
    if (${HEXPOS} LESS ${XRSLEN})
      file(APPEND ${OUTFILE} ",")
    endif ()
  endwhile ()
  file(APPEND ${OUTFILE}
          "};\n"
          "#endif // _${VARNAME}_H_\n")
endfunction()

bin2c("${SRC_FILE}" "${SYMBOL}" "${OUT_FILE}")

