uint32 GZReader::size() {
  return fsize;
}

//This function will allocate memory even if open() fails.
//This is needed so that when SRAM files do not exist, the
//memory for the SRAM data will be allocated still.
//The memory is flushed to 0x00 when no file is opened.
uint8 *GZReader::read(uint32 length) {
uint8 *data;
  if(length == 0) {
  //read the entire file into RAM
    data = (uint8*)malloc(fsize);
    memset(data, 0, fsize);
    if(gp)gzread(gp, data, fsize);
  } else if(length > fsize) {
  //read the entire file into RAM, pad the rest with 0x00s
    data = (uint8*)malloc(length);
    memset(data, 0, length);
    if(gp)gzread(gp, data, fsize);
  } else { //fsize >= length
  //read only what was requested
    data = (uint8*)malloc(length);
    memset(data, 0, length);
    if(gp)gzread(gp, data, length);
  }
  return data;
}

bool GZReader::ready() {
  return (gp != 0);
}

GZReader::GZReader(const char *fn) {
  gp = 0;

FILE *fp = fopen(fn, "rb");
  if(!fp)return;

  fseek(fp, 0, SEEK_END);
  fsize = ftell(fp);

  if(fsize < 4) {
    fclose(fp);
    fp = 0;
    return;
  }

uint32 gzsize;
  fseek(fp, -4, SEEK_END); //jump to 4 bytes before end
  gzsize  = fgetc(fp);
  gzsize |= fgetc(fp) <<  8;
  gzsize |= fgetc(fp) << 16;
  gzsize |= fgetc(fp) << 24;

  fclose(fp);
  fp = 0;

  gp = gzopen(fn, "rb");
  if(!gp)return;

  if(!gzdirect(gp)) {
    fsize = gzsize;
  }

//empty file?
  if(fsize == 0) {
    gzclose(gp);
    gp = 0;
    return;
  }
}

GZReader::~GZReader() {
  if(gp) {
    gzclose(gp);
    gp = 0;
  }
}
