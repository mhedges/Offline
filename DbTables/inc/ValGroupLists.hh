#ifndef DbTables_ValGroupLists_hh
#define DbTables_ValGroupLists_hh


#include <string>
#include <iomanip>
#include <sstream>
#include <map>
#include "DbTables/inc/DbTable.hh"

namespace mu2e {

  class ValGroupLists : public DbTable {
  public:

    class Row {
    public:
      Row(int gid, int iid):
	_gid(gid),_iid(iid) {}
      int  gid() const { return _gid;}
      int  iid() const { return _iid;}
    private:
      int _gid;
      int _iid;
    };


    ValGroupLists():DbTable("ValGroupLists","val.grouplists","gid,iid") {}

    const Row& rowAt(const std::size_t index) const { return _rows.at(index);}
    std::vector<Row> const& rows() const {return _rows;}
    std::size_t nrow() const override { return _rows.size(); };
    size_t size() const override { return baseSize() + sizeof(this) + nrow()*8; };

    void addRow(const std::vector<std::string>& columns) override {
      _rows.emplace_back(std::stoi(columns[0]),std::stoi(columns[1]));
    }

    void rowToCsv(std::ostringstream& sstream, std::size_t irow) const override {
      Row const& r = _rows.at(irow);
      sstream << r.gid()<<",";
      sstream << r.iid();
    }

    virtual void clear() override { baseClear(); _rows.clear(); }

  private:
    std::vector<Row> _rows;
  };
  
};
#endif
