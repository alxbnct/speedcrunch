/* formatbase.hxx: interface of formats */
/*
    Copyright (C) 2008 Wolf Lammen.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License , or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; see the file COPYING.  If not, write to:

      The Free Software Foundation, Inc.
      59 Temple Place, Suite 330
      Boston, MA 02111-1307 USA.


    You may contact the author by:
       e-mail:  ookami1 <at> gmx <dot> de
       mail:  Wolf Lammen
              Oertzweg 45
              22307 Hamburg
              Germany

*************************************************************************/

#ifndef _FORMATBASE_H
#define _FORMATBASE_H

#include "variant/variant.hxx"

typedef const char* FmtType;

typedef enum {
  fmtInclude,
  fmtFixed,
  fmtRelaxed,
} FmtLinkage;

class FormatIntf;

class Format
{
  friend class InitVariant;
  friend class FormatIntf;
  friend class FormatList;
  public:
    Format(const Format&);
    Format(FormatIntf* = 0);
    ~Format();
    bool operator == (const Format& other) const { return p == other.p; }
    bool operator != (const Format& other) const { return p != other.p; }
    Format& operator = (const Format&);
    bool setBase(const QString&, quint64, FmtLinkage);
    FmtType type() const;
    QString format(const Variant&) const;
    bool isValid() const { return p != 0; };
    bool canHandle(VariantType vt) const;
    bool isCompatible(const Format&) const;
    bool isAccessible() const;
    const QStringList* getProps() const;
    bool setProp(const QString& prop, const Variant& val, bool inherit = false);
    Variant getProp(const QString& prop) const;
    static Format find(const QString& key, VariantType);
    static void add(const QString& key, Format);
    static void remove(const QString& key, FmtType aType);
  private:
    static void initClass();
    operator FormatIntf*() const { return p;};
    void operator = (FormatIntf*);
    FormatIntf* p;
};

class FormatIntf
{
  public:
    FormatIntf();
    operator Format() { return Format(this); };
    virtual ~FormatIntf() { releaseBase(); };
    virtual FmtType type() const = 0;
    virtual QString format(const Variant&) const { return QString(); };
    virtual const QStringList& getProps() const = 0;
    virtual bool canHandle(VariantType vt) const { return false; };
    virtual bool isCompatible(FmtType ft) const;
    bool setProp(const QString& prop, const Variant& val, bool inherit);
    Variant getProp(const QString& prop);
    bool setBase(const QString& aBase, quint64 iflags, FmtLinkage = fmtRelaxed);
    bool isBaseImport(const QString& prop) const;
    void lock() { ++refcount; };
    void release();
    QString base() const;
    bool accessible;
  protected:
    virtual bool setValue(int idx, const Variant& val) { return false; };
    virtual Variant getValue(int idx) const = 0;
    void releaseBase();
    void import();
    Format findBase();
    int indexOfProp(const QString& prop) const;
    bool setChar(QChar& dest, const Variant& val);
    static void initClass(QStringList** propList, const char** props);
  private:
    int refcount;
    QString m_baseName;
    Format m_base;
    quint64 m_importmask;
    static quint64 flag(char ofs);
};

extern const char* fmtSettings;

#endif /*_FORMATBASE_H*/
