#ifndef DbTables_ValExtensions_hh
#define DbTables_ValExtensions_hh


#include <string>
#include <iomanip>
#include <sstream>
#include <map>
#include "DbTables/inc/DbTable.hh"

namespace mu2e {

  class ValExtensions : public DbTable {
  public:

    class Row {
    public:
      Row(int eid, int vid, int extension,
	  std::string create_time, std::string create_user):
	_eid(eid),_vid(vid),_extension(extension),
	_create_time(create_time),_create_user(create_user) {}
      int  eid() const { return _eid;}
      int  vid() const { return _vid;}
      int  extension() const { return _extension;}
      std::string const& create_time() const { return _create_time; }
      std::string const& create_user() const { return _create_user; }
    private:
      int _eid;
      int _vid;
      int _extension;
      std::string _create_time;
      std::string _create_user;
    };


    ValExtensions():DbTable("ValExtensions","val.extensions",
		      "eid,vid,extension,create_time,create_user") {}

    const Row& rowAt(const std::size_t index) const { return _rows.at(index);}
    std::vector<Row> const& rows() const {return _rows;}
    std::size_t nrow() const { return _rows.size(); };
    size_t size() const { return sizeof(this) + _csv.capacity() + nrow()*52; };

    void addRow(const std::vector<std::string>& columns) {
      _rows.emplace_back(std::stoi(columns[0]),std::stoi(columns[1]),
			 std::stoi(columns[2]),
			 columns[3],columns[4]);
    }

    void rowToCsv(std::ostringstream& sstream, std::size_t irow) const {
      Row const& r = _rows.at(irow);
      sstream << r.eid()<<",";
      sstream << r.vid()<<",";
      sstream << r.extension()<<",";
      sstream << r.create_time()<<",";
      sstream << r.create_user();
    }

    virtual void clear() { _csv.clear(); _rows.clear(); }

  private:
    std::vector<Row> _rows;
  };
  
};
#endif
