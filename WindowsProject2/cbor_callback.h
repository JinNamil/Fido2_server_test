#pragma once

typedef void(*vpCallBackFunction)(void);
vpCallBackFunction m_pCBFunction;

void SetCallBackFunction(vpCallBackFunction cbFunction)
{
	m_pCBFunction = cbFunction;
}

void ExecutionCallBack(void)
{
	if (m_pCBFunction)
	{
		m_pCBFunction();
	}
}