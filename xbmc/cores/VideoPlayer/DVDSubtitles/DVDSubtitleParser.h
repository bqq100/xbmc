/*
 *      Copyright (C) 2005-2013 Team XBMC
 *      http://kodi.tv
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#pragma once

#include "../DVDCodecs/Overlay/DVDOverlay.h"
#include "DVDSubtitleStream.h"
#include "DVDSubtitleLineCollection.h"

#include <memory>
#include <string>
#include <stdio.h>

class CDVDStreamInfo;

class CDVDSubtitleParser
{
public:
  virtual ~CDVDSubtitleParser() = default;
  virtual bool Open(CDVDStreamInfo &hints) = 0;
  virtual void Dispose() = 0;
  virtual void Reset() = 0;
  virtual CDVDOverlay* Parse(double iPts) = 0;
};

class CDVDSubtitleParserCollection
  : public CDVDSubtitleParser
{
public:
  explicit CDVDSubtitleParserCollection(const std::string& strFile) : m_filename(strFile) {}
  ~CDVDSubtitleParserCollection() override = default;
  CDVDOverlay* Parse(double iPts) override
  {
    CDVDOverlay* o = m_collection.Get(iPts);
    if(o == NULL)
      return o;
    return o->Clone();
  }
  void Reset() override { m_collection.Reset(); }
  void Dispose() override { m_collection.Clear(); }

protected:
  CDVDSubtitleLineCollection m_collection;
  std::string m_filename;
};

class CDVDSubtitleParserText
     : public CDVDSubtitleParserCollection
{
public:
  CDVDSubtitleParserText(std::unique_ptr<CDVDSubtitleStream> && stream, const std::string& filename)
    : CDVDSubtitleParserCollection(filename)
		, m_pStream(std::move(stream))
  {
  }

  ~CDVDSubtitleParserText() override = default;

protected:
  using CDVDSubtitleParserCollection::Open;
  bool Open()
  {
    if(m_pStream)
    {
      if(m_pStream->Seek(0, SEEK_SET) == 0)
        return true;
    }
    else
      m_pStream.reset(new CDVDSubtitleStream());

    return m_pStream->Open(m_filename);
  }

  std::unique_ptr<CDVDSubtitleStream> m_pStream;
};
