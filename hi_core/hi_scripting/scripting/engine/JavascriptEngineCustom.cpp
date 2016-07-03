
struct HiseJavascriptEngine::RootObject::RegisterVarStatement : public Statement
{
	RegisterVarStatement(const CodeLocation& l) noexcept : Statement(l) {}

	ResultCode perform(const Scope& s, var*) const override
	{
		s.root->varRegister.addRegister(name, initialiser->getResult(s));
		return ok;
	}

	Identifier name;
	ExpPtr initialiser;
};


struct HiseJavascriptEngine::RootObject::RegisterAssignment : public Expression
{
	RegisterAssignment(const CodeLocation &l, int registerId, ExpPtr source_) noexcept: Expression(l), registerIndex(registerId), source(source_) {}

	var getResult(const Scope &s) const override
	{
		var value(source->getResult(s));

		s.root->varRegister.setRegister(registerIndex, value);
		return value;

	}

	int registerIndex;

	ExpPtr source;
};


struct HiseJavascriptEngine::RootObject::ApiConstant : public Expression
{
	ApiConstant(const CodeLocation& l) noexcept : Expression(l) {}
	var getResult(const Scope&) const override   { return value; }

	var value;
};

struct HiseJavascriptEngine::RootObject::ApiCall : public Expression
{
	ApiCall(const CodeLocation &l, ApiObject2 *apiClass_, int expectedArguments_, int functionIndex) noexcept:
	Expression(l),
		expectedNumArguments(expectedArguments_),
		functionIndex(functionIndex),
		apiClass(apiClass_)
	{
		for (int i = 0; i < 4; i++)
		{
			results[i] = var::undefined();
			argumentList[i] = nullptr;
		}
	};

	var getResult(const Scope& s) const override
	{
		for (int i = 0; i < expectedNumArguments; i++)
		{
			results[i] = argumentList[i]->getResult(s);
		}

		CHECK_CONDITION(apiClass != nullptr, "API class does not exist");

		return apiClass->callFunction(functionIndex, results, expectedNumArguments);
	}

	const int expectedNumArguments;

	ExpPtr argumentList[4];
	const int functionIndex;
	mutable var results[4];

	const ReferenceCountedObjectPtr<ApiObject2> apiClass;
};

struct HiseJavascriptEngine::RootObject::InlineFunction
{
	struct FunctionCall;

	struct Object : public DynamicObject
	{
	public:

		Object(Identifier &n, const Array<Identifier> &p) : 
			body(nullptr) ,
			name(n),
			e(nullptr)
		{
			parameterNames.addArray(p);
		}

		void setFunctionCall(const FunctionCall *e_)
		{
			e = e_;
		}

		Identifier name;
		Array<Identifier> parameterNames;
		typedef ReferenceCountedObjectPtr<Object> Ptr;
		ScopedPointer<BlockStatement> body;

		const FunctionCall *e;

	};

	struct FunctionCall : public Expression
	{
		FunctionCall(const CodeLocation &l, Object *referredFunction) : 
			Expression(l),
			f(referredFunction),
			numArgs(f->parameterNames.size())
		{
			for (int i = 0; i < numArgs; i++)
			{
				parameterResults.add(var::undefined());
			}
		};

		void addParameter(Expression *e)
		{
			parameterExpressions.add(e);
		}

		var getResult(const Scope& s) const override
		{
			f->setFunctionCall(this);

			for (int i = 0; i < numArgs; i++)
			{
				parameterResults.setUnchecked(i, parameterExpressions.getUnchecked(i)->getResult(s));
			}

			ResultCode c = f->body->perform(s, &returnVar);

			if (c == Statement::returnWasHit) return returnVar;
			else return var::undefined();
		}

		Object::Ptr f;

		OwnedArray<Expression> parameterExpressions;
		mutable Array<var> parameterResults;

		mutable var returnVar;

		const int numArgs;
	};

	struct ParameterReference : public Expression
	{
		ParameterReference(const CodeLocation &l, Object *referedFunction, int id):
			Expression(l),
			index(id),
			f(referedFunction)
		{}

		var getResult(const Scope&) const override   { return  (f->e->parameterResults.getUnchecked(index)); }

		Object::Ptr f;
		int index;
	};
};