#include "rlbs_reactor.h"
#include "rlbs.pb.h"
#include "mysql.h"

class StoreReport
{
public:	
	StoreReport();

	void store(rlbs::ReportStatusRequest& req);

private:
	MYSQL _db_conn;
};

void* store_main(void*);
