#include "TestsRunner.h"

#include <iostream>

using namespace std;

TestsRunner::TestsRunner(void)
{
}

TestsRunner::~TestsRunner(void)
{
	DeleteTests();
}

void TestsRunner::DeleteTests()
{
	WStringToWStringAndTestUnitListMap::iterator it = mTests.begin();
	for(;it!= mTests.end();++it)
	{
		WStringAndTestUnitList::iterator itList = it->second.begin();
		for(; itList != it->second.end();++itList)
			delete(itList->second);
	}
	mTests.clear();
}

EStatusCode TestsRunner::RunAll()
{
	WStringAndTestUnitList testsList;
	WStringToTestUnitMap::iterator it = mTestsByName.begin();
	for(;it != mTestsByName.end();++it)
		testsList.push_back(WStringAndTestUnit(it->first,it->second));
	return RunTestsInList(testsList);
}

EStatusCode TestsRunner::RunTestsInList(const WStringAndTestUnitList& inTests)
{
	EStatusCode testsStatus;

	if(inTests.size() == 0)
	{
		wcout<<L"No tests to run\n";
		testsStatus = eSuccess;
	}
	else if(inTests.size() == 1)
	{
		testsStatus = RunSingleTest(inTests.front().first,inTests.front().second);
	}
	else
	{
		unsigned long failedCount = 0,succeededCount = 0;
		WStringAndTestUnitList::const_iterator it = inTests.begin();
		EStatusCode testStatus;
		testsStatus = eSuccess;

		wcout<<L"Start tests run\n";
		for(;it!= inTests.end();++it)
		{
			testStatus = RunSingleTest(it->first,it->second);
			if(eFailure == testStatus)
			{
				++failedCount;
				testsStatus = eFailure;
			}
			else
			{
				++succeededCount;
			}
		}
		wcout<<L"--------------------------------------------------------------\n";
		wcout<<L"Test run ended: "<<succeededCount<<L" tests succeeded, "<<failedCount<<L" tests failed.\n";
	}	
	return testsStatus;
}

EStatusCode TestsRunner::RunSingleTest(const wstring& inTestName,ITestUnit* inTest)
{
	EStatusCode testStatus;
	
	wcout<<L"Running Test "<<inTestName<<L"\n";
	testStatus = inTest->Run();
	if(eFailure == testStatus)
		wcout<<L"Test "<<inTestName<<L" Failed\n\n";
	else
		wcout<<L"Test "<<inTestName<<L" Succeeded\n\n";
	return testStatus;
}

void TestsRunner::AddTest(const std::wstring& inTestLabel,const std::wstring& inCategory,ITestUnit* inTest)
{
	WStringToWStringAndTestUnitListMap::iterator it = mTests.find(inCategory);
	
	if(it == mTests.end())
		it = mTests.insert(WStringToWStringAndTestUnitListMap::value_type(inCategory,WStringAndTestUnitList())).first;
	it->second.push_back(WStringAndTestUnit(inTestLabel,inTest));
	mTestsByName.insert(WStringToTestUnitMap::value_type(inTestLabel,inTest));
}

static const wstring scGeneral= L"General";
void TestsRunner::AddTest(const wstring& inTestLabel,ITestUnit* inTest)
{
	AddTest(inTestLabel,scGeneral,inTest);
}

EStatusCode TestsRunner::RunTest(const wstring& inTestLabel)
{
	WStringToTestUnitMap::iterator it = mTestsByName.find(inTestLabel);

	if(it == mTestsByName.end())
	{
		wcout<<L"Test not found\n";
		return eSuccess;
	}
	else
		return RunSingleTest(it->first,it->second);
}

EStatusCode TestsRunner::RunTests(const WStringList& inTestsLabels)
{
	WStringAndTestUnitList testsList;
	WStringList::const_iterator it = inTestsLabels.begin();
	for(; it != inTestsLabels.end(); ++it)
	{
		WStringToTestUnitMap::iterator itMap = mTestsByName.find(*it);
		if(itMap != mTestsByName.end())
			testsList.push_back(WStringAndTestUnit(itMap->first,itMap->second));
		else
			wcout<<L"Test "<<*it<<" not found\n";
	}
	return RunTestsInList(testsList);
}

EStatusCode TestsRunner::RunCategory(const wstring& inCategory)
{
	WStringToWStringAndTestUnitListMap::iterator it = mTests.find(inCategory);

	if(it == mTests.end())
	{
		wcout<<L"Category "<<inCategory<<L"not found\n";
		return eSuccess;
	}
	else
		return RunTestsInList(it->second);
}

EStatusCode TestsRunner::RunCategories(const WStringList& inCategories)
{
	WStringAndTestUnitList testsList;
	WStringList::const_iterator it = inCategories.begin();

	for(; it != inCategories.end(); ++it)
	{
		WStringToWStringAndTestUnitListMap::iterator itMap = mTests.find(*it);
		if(itMap != mTests.end())
			testsList.insert(testsList.end(),itMap->second.begin(),itMap->second.end());
		else
			wcout<<L"Category "<<*it<<" not found\n";
	}

	return RunTestsInList(testsList);
}

EStatusCode TestsRunner::RunCategories(const WStringList& inCategories, const WStringSet& inTestsToExclude)
{
	WStringAndTestUnitList testsList;
	WStringList::const_iterator it = inCategories.begin();

	for(; it != inCategories.end(); ++it)
	{
		WStringToWStringAndTestUnitListMap::iterator itMap = mTests.find(*it);
		if(itMap != mTests.end())
		{
			WStringAndTestUnitList::iterator itCategoryTests = itMap->second.begin();
			for(;itCategoryTests != itMap->second.end();++itCategoryTests)
				if(inTestsToExclude.find(itCategoryTests->first) == inTestsToExclude.end())
					testsList.push_back(WStringAndTestUnit(itCategoryTests->first,itCategoryTests->second));
		}
		else
			wcout<<L"Category "<<*it<<" not found\n";
	}

	return RunTestsInList(testsList);
}


EStatusCode TestsRunner::RunExcludeCategories(const WStringSet& inCategories)
{
	WStringAndTestUnitList testsList;
	WStringToWStringAndTestUnitListMap::iterator it = mTests.begin();

	for(; it != mTests.end(); ++it)
		if(inCategories.find(it->first) == inCategories.end())
			testsList.insert(testsList.end(),it->second.begin(),it->second.end());
	return RunTestsInList(testsList);
}

EStatusCode TestsRunner::RunExcludeTests(const WStringSet& inTests)
{
	WStringAndTestUnitList testsList;
	WStringToTestUnitMap::iterator it = mTestsByName.begin();

	for(; it != mTestsByName.end();++it)
		if(inTests.find(it->first) == inTests.end())
			testsList.push_back(WStringAndTestUnit(it->first,it->second));

	return RunTestsInList(testsList);
}