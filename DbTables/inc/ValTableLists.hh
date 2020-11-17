#ifndef DbTables_ValTableLists_hh
#define DbTables_ValTableLists_hh


#include <string>
#include <iomanip>
#include <sstream>
#include <map>
#include "DbTables/inc/DbTable.hh"

namespace mu2e {

  class ValTableLists : public DbTable {
  public:

    class Row {
    public:
      Row(int lid, int tid):
	_lid(lid),_tid(tid) {}
      int  lid() const { return _lid;}
      int  tid() const { return _tid;}
    private:
      int _lid;
      int _tid;
    };


    ValTableLists():DbTable("ValTableLists","val.tablelists","lid,tid") {}

    const Row& rowAt(const std::size_t index) const { return _rows.at(index);}
    std::vector<Row> const& rows() const {return _rows;}
    std::size_t nrow() const override { return _rows.size(); };
    size_t size() const override { return baseSize() + sizeof(this) + nrow()*8; };

    void addRow(const std::vector<std::string>& columns) override {
      _rows.emplace_back(std::stoi(columns[0]),std::stoi(columns[1]));
    }

    void rowToCsv(std::ostringstream& sstream, std::size_t irow) const override {
      Row const& r = _rows.at(irow);
      sstream << r.lid()<<",";
      sstream << r.tid();
    }

    virtual void clear() override { baseClear(); _rows.clear(); }

  private:
    std::vector<Row> _rows;
  };
  
};
#endif
