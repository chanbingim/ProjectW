#pragma once
#include <functional>
#include <map>
#include <string>

class CreateStateParameter
{
public:
	std::function<void(class StateMachine* _Parent)> Init;
	std::function<void(class StateMachine* _Parent)> Start;
	std::function<void(float DeltaTime, class StateMachine* _Parent)> Stay;
	std::function<void(class StateMachine* _Parent)> End;
};

class State
{
public:
	std::string Name;

	CreateStateParameter Event;
};

class StateMachine
{
public:
	template<typename EnumType>
	void ChangeState(EnumType _Stateindex)
	{
		return ChangeState(static_cast<int>(_Stateindex));
	}

	void ChangeState(int _Stateindex)
	{
		if (CurState != nullptr && CurState->Event.End != nullptr)
		{
			CurState->Event.End(this);
		}

		StateTime = 0.f;
		CurState = &States[_Stateindex];

		if (CurState != nullptr && CurState->Event.Start != nullptr)
		{
			CurState->Event.Start(this);
		}
	}

	template<typename EnumType>
	void CreateState(EnumType _Stateindex, const CreateStateParameter& _Para)
	{
		return CreateState(static_cast<int>(_Stateindex), _Para);
	}

	void CreateState(int _Stateindex, const CreateStateParameter& _Para)
	{
		States[_Stateindex].Event = _Para;
	}

	template<typename EnumType>
	void Find(EnumType _Stateindex)
	{
		return Find(static_cast<int>(_Stateindex));
	}

	State* Find(int _Index)
	{
		if (false == States.contains(_Index))
		{
			return nullptr;
		}
		return &States[_Index];
	}

	void Update(float _DeltaTime)
	{
		if (CurState == nullptr || CurState->Event.Stay == nullptr)
		{
			return;
		}

		StateTime += _DeltaTime;

		CurState->Event.Stay(_DeltaTime, this);
	}

	float GetStateTime() { return StateTime; }

protected:

private:
	State* CurState = nullptr;
	std::map<int, State> States;
	float StateTime = 0.f;
};