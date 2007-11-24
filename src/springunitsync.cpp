/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */

#include <wx/dynlib.h>
#include <wx/filefn.h>
#include <wx/filename.h>
#include <wx/image.h>
#include <wx/intl.h>
#include <wx/msgdlg.h>
#include <wx/mstream.h>
#include <wx/stdpaths.h>
#include <wx/string.h>
#include <wx/file.h>
//#include <wx/txtstrm.h>
//#include <wx/wfstream.h>
#include <wx/textfile.h>

#include <stdexcept>

#include "springunitsync.h"
#include "utils.h"
#include "settings.h"
#include "springunitsynclib.h"


#define LOCK_UNITSYNC wxCriticalSectionLocker lock_criticalsection(m_lock)


struct SpringMapInfo
{
  char* description;
  int tidalStrength;
  int gravity;
  float maxMetal;
  int extractorRadius;
  int minWind;
  int maxWind;

  int width;
  int height;
  int posCount;
  StartPos positions[16];

  char* author;
};


struct CachedMapInfo
{
  char name[256];
  char author[256];
  char description[256];

  int tidalStrength;
  int gravity;
  float maxMetal;
  int extractorRadius;
  int minWind;
  int maxWind;

  int width;
  int height;

  int posCount;
  StartPos positions[16];
};


IUnitSync* usync()
{
  static SpringUnitSync* m_sync = 0;
  if (!m_sync)
    m_sync = new SpringUnitSync;
  return m_sync;
}


bool SpringUnitSync::LoadUnitSyncLib( const wxString& springdir, const wxString& unitsyncloc )
{
  debug_func("");
  LOCK_UNITSYNC;
  return _LoadUnitSyncLib( springdir, unitsyncloc );
}


bool SpringUnitSync::_LoadUnitSyncLib( const wxString& springdir, const wxString& unitsyncloc )
{
  wxSetWorkingDirectory( springdir );
  try {
    susynclib()->Load( unitsyncloc );
  } catch (...) {
    return false;
  }
  return true;
}


void SpringUnitSync::FreeUnitSyncLib()
{
  debug_func( "" );
  susynclib()->Unload();
}


bool SpringUnitSync::IsLoaded()
{
  return susynclib()->IsLoaded();
}


std::string SpringUnitSync::GetSpringVersion()
{
  debug_func("");
  return STD_STRING(susynclib()->GetSpringVersion());
}


int SpringUnitSync::GetNumMods()
{
  debug_func("");
  return susynclib()->GetPrimaryModCount();
}


int SpringUnitSync::GetModIndex( const std::string& name )
{
  debug_func( "name = \"" + name + "\"" );
  return _GetModIndex( name );
}


int SpringUnitSync::_GetModIndex( const std::string& name )
{
  try {
    int count = susynclib()->GetPrimaryModCount();
    for ( int i = 0; i < count; i++ ) {
      std::string cmp = STD_STRING(susynclib()->GetPrimaryModName( i ));
      if ( name == cmp ) return i;
    }
  } catch (...) {}
  return -1;
}


bool SpringUnitSync::ModExists( const std::string& modname )
{
  debug_func( "modname = \"" + modname + "\"" );
  try {
    return susynclib()->GetPrimaryModIndex( WX_STRING(modname) ) >= 0;
  } catch (...) {}
  return false;
}


UnitSyncMod SpringUnitSync::GetMod( const std::string& modname )
{
  debug_func( "modname = \"" + modname + "\"" );
  UnitSyncMod m;

  int i = susynclib()->GetPrimaryModIndex( WX_STRING(modname) );
  return GetMod( i );
}


UnitSyncMod SpringUnitSync::GetMod( int index )
{
  debug_func( "" );
  UnitSyncMod m;

  m.name = STD_STRING(susynclib()->GetPrimaryModName( index ));
  m.hash = i2s(susynclib()->GetPrimaryModChecksum( index ));
}


int SpringUnitSync::GetNumMaps()
{
  debug_func( "" );
  return susynclib()->GetMapCount();
}


bool SpringUnitSync::MapExists( const std::string& mapname )
{
  debug_func( "" );
  try {
    return GetMapIndex( mapname ) >= 0;
  } catch (...) {}
  return false;
}


bool SpringUnitSync::MapExists( const std::string& mapname, const std::string hash )
{
  debug_func( "" );
  try {
    int i = GetMapIndex( mapname );
    if ( i >= 0 ) {
      return ( i2s(susynclib()->GetMapChecksum( i )) == hash );
    }
  } catch (...) {}
  return false;
}


UnitSyncMap SpringUnitSync::GetMap( const std::string& mapname, bool getmapinfo )
{
  int i = GetMapIndex( mapname );
  return GetMap( i, getmapinfo );
}


MapInfo SpringUnitSync::_GetMapInfoEx( const std::string& mapname )
{
  debug_func("");
  MapCacheType::iterator i = m_mapinfo.find(mapname);
  if ( i != m_mapinfo.end() ) {
    debug("GetMapInfoEx cache lookup.");
    MapInfo info;
    CachedMapInfo cinfo = i->second;
    _ConvertSpringMapInfo( cinfo, info );
    return info;
  }

  debug("GetMapInfoEx cache lookup failed.");

  char tmpdesc[256];
  char tmpauth[256];

  SpringMapInfo tm;
  tm.description = &tmpdesc[0];
  tm.author = &tmpauth[0];

  tm = susynclib()->GetMapInfoEx( WX_STRING(mapname), 0 );

  MapInfo info;
  _ConvertSpringMapInfo( tm, info );

  CachedMapInfo cinfo;
  _ConvertSpringMapInfo( tm, cinfo, mapname );
  m_mapinfo[mapname] = cinfo;

  return info;
}


UnitSyncMap SpringUnitSync::GetMap( int index, bool getmapinfo )
{
  UnitSyncMap m;
  m.name = STD_STRING(susynclib()->GetMapName( index ));
  m.hash = i2s(susynclib()->GetMapChecksum( index ));
  if ( getmapinfo ) m.info = _GetMapInfoEx( m.name );
  return m;
}


int SpringUnitSync::_GetMapIndex( const std::string& name )
{
  try {
    int count = susynclib()->GetMapCount();
    for ( int i = 0; i < count; i++ ) {
      std::string cmp = STD_STRING(susynclib()->GetMapName( i ));
      if ( name == cmp )
        return i;
    }
  } catch(...) {}
  return -1;
}


std::string SpringUnitSync::GetModArchive( int index )
{
  debug_func( "" );
  LOCK_UNITSYNC;

  return _GetModArchive( index );
}


std::string SpringUnitSync::_GetModArchive( int index )
{
  return STD_STRING(susynclib()->GetPrimaryModArchive( index ));
}


int SpringUnitSync::GetSideCount( const std::string& modname )
{
  debug_func( modname );

  if ( !_ModExists( modname ) ) return 0;
  return susynclib()->GetSideCount();
}


std::string SpringUnitSync::GetSideName( const std::string& modname, int index )
{
  debug_func( "" );

  if ( (index < 0) || (!_ModExists( modname )) ) return "unknown";
  susynclib()->AddAllArchives( _GetModArchive( _GetModIndex( modname ) ) );
  if ( index >= GetSideCount( modname ) ) return "unknown";
  ASSERT_LOGIC( GetSideCount( modname ) > index, "Side index too high." );
  return STD_STRING(susynclib()->GetSideName( index ));
}


wxImage SpringUnitSync::GetSidePicture( const std::string& modname, const std::string& SideName )
{
  debug_func( "" );

  susynclib()->AddAllArchives( _GetModArchive( _GetModIndex( modname ) ) );
  debug_func( "SideName = \"" + SideName + "\"" );
  wxString ImgName = _T("SidePics");
  ImgName += _T("/");
  ImgName += WX_STRING( SideName ).Upper();
  ImgName += _T(".bmp");

  int ini = susynclib()->OpenFileVFS (ImgName );
  ASSERT_RUNTIME( ini, "cannot find side image" );

  int FileSize = susynclib()->FileSizeVFS(ini);
  if (FileSize == 0) {
    susynclib()->CloseFileVFS(ini);
    ASSERT_RUNTIME( FileSize, "side image has size 0" );
  }

  char* FileContent = new char [FileSize];
  susynclib()->ReadFileVFS(ini, FileContent, FileSize);
  wxMemoryInputStream FileContentStream( FileContent, FileSize );

  wxImage ret( FileContentStream, wxBITMAP_TYPE_ANY, -1);
  delete[] FileContent;
  return ret;
}


wxArrayString SpringUnitSync::GetAIList()
{
  debug_func( "" );

  int ini = susynclib()->InitFindVFS( "AI/Bot-libs/*" );
  bool more;
  wxString FileName;
  wxArrayString ret;

  do
  {
    more = susynclib()->FindFilesVFS( ini, FileName );
    if ( !FileName.Contains ( _T(".dll") ) && !FileName.Contains (  _T(".so") ) ) continue; // FIXME this isn't exactly portable
    if ( ret.Index( FileName.BeforeLast( '/') ) == wxNOT_FOUND ) ret.Add ( FileName ); // don't add duplicates
  } while ( !more );

  const int LuaAICount = susynclib()->GetLuaAICount();
  for ( int i = 0; i < LuaAICount; i++ ) ret.Add( _( "LuaAI" ) +  susynclib()->GetLuaAIName( i ) );

  return ret;
}


int SpringUnitSync::GetNumUnits( const std::string& modname )
{
  debug_func( "" );

  susynclib()->AddAllArchives( _GetModArchive( _GetModIndex( modname ) ) );
  susynclib()->ProcessUnitsNoChecksum();

  return susynclib()->GetUnitCount();
}


wxString _GetCachedModUnitsFileName( const wxString& mod )
{
  wxString path = sett().GetCachePath(); //wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + _T("cache") + wxFileName::GetPathSeparator();
  wxString fname = WX_STRING( mod );
  fname.Replace( _T("."), _T("_") );
  fname.Replace( _T(" "), _T("_") );
  debug(STD_STRING(path));
  return path + fname + _T(".units");
}


wxArrayString SpringUnitSync::GetUnitsList( const std::string& modname )
{
  debug_func( "" );

  if ( m_mod_units.GetCount() > 0 ) return m_mod_units;

  wxArrayString ret;

  wxString path = _GetCachedModUnitsFileName( WX_STRING( modname ) );
  try {

    ASSERT_RUNTIME( wxFileName::FileExists( path ), "Cache file does not exist" );
    wxTextFile f;
    ASSERT_RUNTIME( f.Open(path), "Failed to open file" );
    ASSERT_RUNTIME( f.GetLineCount() > 0, "File empty" );

    wxString str;
    for ( str = f.GetFirstLine(); !f.Eof(); str = f.GetNextLine() ) ret.Add( str );

    return ret;

  } catch(...) {}

  susynclib()->AddAllArchives( _GetModArchive( _GetModIndex( modname ) ) );
  while ( susynclib()->ProcessUnitsNoChecksum() );
  for ( int i = 0; i < susynclib()->GetUnitCount(); i++ ) {
    wxString tmp = susynclib()->GetUnitName(i) + _T("(");
    tmp += susynclib()->GetUnitName(i) + _T(")");
    ret.Add( tmp );
  }

  m_mod_units = ret;
  try {

    wxFile f( path, wxFile::write );
    ASSERT_RUNTIME( f.IsOpened(), "Couldn't create file" );

    for ( unsigned int i = 0; i < ret.GetCount(); i++ ) {
      std::string tmp = STD_STRING( ret.Item(i) );
      tmp += "\n";
      f.Write( tmp.c_str(), tmp.length() );
    }

    f.Close();

  } catch(...) {}

  return ret;
}


wxString SpringUnitSync::_GetCachedMinimapFileName( const std::string& mapname, int width, int height )
{
  wxString path = sett().GetCachePath(); //wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + _T("cache") + wxFileName::GetPathSeparator();
  wxString fname = WX_STRING( mapname );
  fname.Replace( _T("."), _T("_") );
  fname.Replace( _T(" "), _T("_") );
  if ( width != -1 ) fname += wxString::Format( _T("%dx%d"), width, height );
  fname += _T(".png");
  return path + fname;
}


wxImage SpringUnitSync::_GetCachedMinimap( const std::string& mapname, int max_w, int max_h, bool store_size )
{
  wxString fname = store_size? _GetCachedMinimapFileName( mapname, max_w, max_h ) : _GetCachedMinimapFileName( mapname );
  ASSERT_RUNTIME( wxFileExists( fname ), "File cached image does not exist" );

  wxImage img( fname, wxBITMAP_TYPE_PNG );
  ASSERT_RUNTIME( img.Ok(), "Failed to load chache image" );

  if ( !store_size ) {

    UnitSyncMap map = _GetMap( mapname );
    if ( map.hash != m_map.hash ) map = m_map = _GetMap( mapname, true );
    else map = m_map;

    int height, width;

    width = max_w;
    height = (int)((double)((double)max_w * (double)map.info.height) / (double)map.info.width);
    if ( height > max_h ) {
      width = (int)((double)((double)width * (double)max_h) / (double)height);
      height = max_h;
    }

    img.Rescale( width, height );

  }

  return img;
}


wxImage SpringUnitSync::GetMinimap( const std::string& mapname, int max_w, int max_h, bool store_size )
{
  debug_func( "" );

  int height = 1024;
  int width = 512;

  try {
    return _GetCachedMinimap( mapname, max_w, max_h, store_size );
  } catch(...) {
    debug( "Cache lookup failed." );
  }

  wxImage ret = susynclib()->GetMinimap( WX_STRING(mapname), 0 );


  UnitSyncMap map = _GetMap( mapname, true );

  width = max_w;
  height = (int)((double)((double)max_w * (double)map.info.height) / (double)map.info.width);
  if ( height > max_h ) {
    width = (int)((double)((double)width * (double)max_h) / (double)height);
    height = max_h;
  }

  ret.Rescale( 512, 512 );

  wxString fname = _GetCachedMinimapFileName( mapname );
  if ( !wxFileExists( fname ) ) ret.SaveFile( fname, wxBITMAP_TYPE_PNG );

  ret.Rescale( width, height );

  if ( store_size ) {
    ret.SaveFile( _GetCachedMinimapFileName( mapname, max_w, max_h ), wxBITMAP_TYPE_PNG );
  }

  return ret;
}


void SpringUnitSync::_ConvertSpringMapInfo( const SpringMapInfo& in, MapInfo& out )
{
  out.author = in.author;
  out.description = in.description;

  out.extractorRadius = in.extractorRadius;
  out.gravity = in.gravity;
  out.tidalStrength = in.tidalStrength;
  out.maxMetal = in.maxMetal;
  out.minWind = in.minWind;
  out.maxWind = in.maxWind;

  out.width = in.width;
  out.height = in.height;
  out.posCount = in.posCount;
  for ( int i = 0; i < in.posCount; i++) out.positions[i] = in.positions[i];
}


void SpringUnitSync::_ConvertSpringMapInfo( const CachedMapInfo& in, MapInfo& out )
{
  out.author = in.author;
  out.description = in.description;

  out.extractorRadius = in.extractorRadius;
  out.gravity = in.gravity;
  out.tidalStrength = in.tidalStrength;
  out.maxMetal = in.maxMetal;
  out.minWind = in.minWind;
  out.maxWind = in.maxWind;

  out.width = in.width;
  out.height = in.height;
  out.posCount = in.posCount;
  for ( int i = 0; i < in.posCount; i++) out.positions[i] = in.positions[i];
}


void SpringUnitSync::_ConvertSpringMapInfo( const SpringMapInfo& in, CachedMapInfo& out, const std::string& mapname )
{
  strncpy( &out.name[0], mapname.c_str(), 256 );
  strncpy( &out.author[0], in.author, 256 );
  strncpy( &out.description[0], in.description, 256 );

  out.tidalStrength = in.tidalStrength;
  out.gravity = in.gravity;
  out.maxMetal = in.maxMetal;
  out.extractorRadius = in.extractorRadius;
  out.minWind = in.minWind;
  out.maxWind = in.maxWind;

  out.width = in.width;
  out.height = in.height;

  out.posCount = in.posCount;
  for ( int i = 0; i < 16; i++ ) out.positions[i] = in.positions[i];
}


void SpringUnitSync::_LoadMapInfoExCache()
{
  debug_func("");

  wxString path = sett().GetCachePath() + _T("mapinfoex.cache"); //wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + _T("cache") + wxFileName::GetPathSeparator() + _T("mapinfoex.cache");

  if ( !wxFileName::FileExists( path ) ) {
    debug( "No cache file found." );
    return;
  }

  wxFile f( path.c_str(), wxFile::read );
  if ( !f.IsOpened() ) {
    debug( "failed to open file for reading." );
    return;
  }

  m_mapinfo.clear();

  CachedMapInfo cinfo;
  while ( !f.Eof() ) {
    if ( (unsigned int)f.Read( &cinfo, sizeof(CachedMapInfo) ) < sizeof(CachedMapInfo) ) {
      debug_error( "Cache file invalid" );
      m_mapinfo.clear();
      break;
    }
    m_mapinfo[ std::string( &cinfo.name[0] ) ] = cinfo;
  }
  f.Close();
}


void SpringUnitSync::_SaveMapInfoExCache()
{
  debug_func("");
  wxString path = sett().GetCachePath() + _T("mapinfoex.cache"); //wxStandardPaths::Get().GetUserDataDir() + wxFileName::GetPathSeparator() + _T("cache") + wxFileName::GetPathSeparator() + _T("mapinfoex.cache");

  wxFile f( path.c_str(), wxFile::write );
  if ( !f.IsOpened() ) {
    debug( "failed to open file for writing." );
    return;
  }

  MapCacheType::iterator i = m_mapinfo.begin();
  while ( i != m_mapinfo.end() ) {
    f.Write( &i->second, sizeof(CachedMapInfo) );
    i++;
  }
  f.Close();
}



