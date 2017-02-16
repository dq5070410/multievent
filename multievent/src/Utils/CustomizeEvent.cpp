#include "Base.h"
#include "CustomizeEvent.h"

ME_NAME_SPACE_BEGIN

int ICustomizeEvent::OnDestroy()
{
	delete this;

	return ME_OK;
}

ME_NAME_SPACE_END









