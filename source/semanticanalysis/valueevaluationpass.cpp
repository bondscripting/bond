namespace Bond
{

class ValueEvaluationPass: public SemanticAnalysisPass
{
public:
	ValueEvaluationPass(ParseErrorBuffer &errorBuffer, SymbolTable &symbolTable, bu32_t pointerSize):
		SemanticAnalysisPass(errorBuffer, symbolTable),
		mPrevEnumerator(0),
		mPointerSize(pointerSize),
		mHasResolvedItems(false),
		mHasUnresolvedItems(false)
	{}

	virtual ~ValueEvaluationPass() {}

	virtual void Analyze(TranslationUnit *translationUnitList);

protected:
	virtual void Visit(EnumDeclaration *enumDeclaration);
	virtual void Visit(Enumerator *enumerator);
	virtual void Visit(StructDeclaration *structDeclaration);
	virtual void Visit(Parameter *parameter);
	virtual void Visit(TypeDescriptor *typeDescriptor);
	virtual void Visit(NamedInitializer *namedInitializer);
	virtual void Visit(SwitchLabel *switchLabel);
	virtual void Visit(DeclarativeStatement *declarativeStatement);
	virtual void Visit(ConditionalExpression *conditionalExpression);
	virtual void Visit(BinaryExpression *binaryExpression);
	virtual void Visit(UnaryExpression *unaryExpression);
	virtual void Visit(PostfixExpression *postfixExpression);
	virtual void Visit(MemberExpression *memberExpression);
	virtual void Visit(ArraySubscriptExpression *arraySubscriptExpression);
	virtual void Visit(FunctionCallExpression *functionCallExpression);
	virtual void Visit(CastExpression *castExpression);
	virtual void Visit(SizeofExpression *sizeofExpression);
	virtual void Visit(ConstantExpression *constantExpression);
	virtual void Visit(IdentifierExpression *identifierExpression);

private:
	struct AlignmentComparator
	{
		bool operator()(const DeclarativeStatement &a, const DeclarativeStatement &b) const;
	};

	void Resolve(TypeAndValue &tav);
	void CheckUnresolved(const TypeAndValue &tav);

	ParseErrorBuffer mUnresolvedErrorBuffer;
	Enumerator *mPrevEnumerator;
	bu32_t mPointerSize;
	bool mHasResolvedItems;
	bool mHasUnresolvedItems;
};


void ValueEvaluationPass::Analyze(TranslationUnit *translationUnitList)
{
	do
	{
		mHasResolvedItems = false;
		mHasUnresolvedItems = false;
		mUnresolvedErrorBuffer.Reset();
		SemanticAnalysisPass::Analyze(translationUnitList);
	}
	while (mHasResolvedItems && mHasUnresolvedItems && !mErrorBuffer.HasErrors());

	if (!mErrorBuffer.HasErrors())
	{
		// If something has not been resolved but no errors were reported, then
		// the compiler did something wrong.
		if (mHasUnresolvedItems && !mUnresolvedErrorBuffer.HasErrors())
		{
			mErrorBuffer.PushError(ParseError::INTERNAL_ERROR);
		}

		mErrorBuffer.CopyFrom(mUnresolvedErrorBuffer);
	}
}


void ValueEvaluationPass::Visit(EnumDeclaration *enumDeclaration)
{
	mPrevEnumerator = 0;
	ParseNodeTraverser::Visit(enumDeclaration);
	mPrevEnumerator = 0;
}


void ValueEvaluationPass::Visit(Enumerator *enumerator)
{
	TypeAndValue &tav = *enumerator->GetTypeAndValue();
	if (!tav.IsResolved())
	{
		ParseNodeTraverser::Visit(enumerator);

		if (enumerator->GetValue() != 0)
		{
			const TypeAndValue &valueTav = enumerator->GetValue()->GetTypeAndValue();
			if (valueTav.IsValueDefined())
			{
				Resolve(tav);
				const TypeDescriptor *resultType = tav.GetTypeDescriptor();
				tav.SetValue(CastValue(valueTav, resultType));
			}
		}
		else
		{
			if (mPrevEnumerator != 0)
			{
				const TypeAndValue &prevTav = *mPrevEnumerator->GetTypeAndValue();
				if (prevTav.IsValueDefined())
				{
					Resolve(tav);
					tav.SetIntValue(prevTav.GetIntValue() + 1);
				}
			}
			else
			{
				Resolve(tav);
				tav.SetIntValue(0);
			}
		}

		CheckUnresolved(tav);
	}

	mPrevEnumerator = enumerator;
}


void ValueEvaluationPass::Visit(StructDeclaration *structDeclaration)
{
	SemanticAnalysisPass::Visit(structDeclaration);
	if (!structDeclaration->IsResolved())
	{
		switch (structDeclaration->GetVariant())
		{
			case StructDeclaration::VARIANT_BOND:
			{
				bool membersResolved = true;
				DeclarativeStatement *memberList = structDeclaration->GetMemberVariableList();
				while (memberList != 0)
				{
					if (!memberList->GetTypeDescriptor()->IsResolved())
					{
						membersResolved = false;
						break;
					}
					memberList = NextNode(memberList);
				}

				if (membersResolved)
				{
					memberList = structDeclaration->GetMemberVariableList();
					memberList = Sort<DeclarativeStatement, AlignmentComparator>(memberList);
					structDeclaration->SetMemberVariableList(memberList);

					bu32_t structSize = 0;
					bu32_t structAlign = 1;
					while (memberList != 0)
					{
						const TypeDescriptor *memberDescriptor = memberList->GetTypeDescriptor();
						const bu32_t memberSize = memberDescriptor->GetSize(mPointerSize);
						const bu32_t memberAlign = memberDescriptor->GetAlignment();

						structSize = Align(structSize, memberAlign);
						structAlign = Max(structAlign, memberAlign);

						NamedInitializer *initializerList = memberList->GetNamedInitializerList();
						while (initializerList != 0)
						{
							initializerList->SetOffset(structSize);
							structSize += memberSize;
							initializerList = NextNode(initializerList);
						}

						memberList = NextNode(memberList);
					}

					structSize = Align(structSize, structAlign);
					structDeclaration->SetSize(structSize);
					structDeclaration->SetAlignment(structAlign);
				}
			}
			break;

			case StructDeclaration::VARIANT_NATIVE:
			{
				bool hasError = false;
				const Token *sizeToken = structDeclaration->GetSizeToken();
				const bi32_t size = CastValue(sizeToken->GetValue(), sizeToken->GetTokenType(), Token::CONST_INT).mInt;
				if (size <= 0)
				{
					hasError = true;
					mErrorBuffer.PushError(ParseError::INVALID_STRUCT_SIZE, sizeToken);
				}
				else
				{
					structDeclaration->SetSize(static_cast<bu32_t>(size));
				}

				const Token *alignToken = structDeclaration->GetAlignmentToken();
				if (alignToken != 0)
				{
					const bi32_t align = CastValue(alignToken->GetValue(), alignToken->GetTokenType(), Token::CONST_INT).mInt;
					if ((align <= 1) || !IsPowerOfTwo(align))
					{
						hasError = true;
						mErrorBuffer.PushError(ParseError::INVALID_STRUCT_ALIGNMENT, alignToken);
					}
					else
					{
						structDeclaration->SetAlignment(static_cast<bu32_t>(align));
					}
				}

				if (!hasError && ((structDeclaration->GetSize() % structDeclaration->GetAlignment()) != 0))
				{
					mErrorBuffer.PushError(ParseError::STRUCT_SIZE_ALIGNMENT_MISMATCH, sizeToken);
				}
			}
			break;

			default:
				break;
		}
	}
}


void ValueEvaluationPass::Visit(Parameter *parameter)
{
	ParseNodeTraverser::Visit(parameter);
	Resolve(*parameter->GetTypeAndValue());
}


void ValueEvaluationPass::Visit(TypeDescriptor *typeDescriptor)
{
	if (!typeDescriptor->IsResolved())
	{
		ParseNodeTraverser::Visit(typeDescriptor);

		Expression *expressionList = typeDescriptor->GetLengthExpressionList();
		while (expressionList != 0)
		{
			// Validate the value and cast it to an unsigned integar.
			TypeAndValue &tav = expressionList->GetTypeAndValue();
			if (tav.IsResolved())
			{
				const bool isUInt = tav.GetTypeDescriptor()->GetPrimitiveType() == Token::KEY_UINT;

				if (tav.IsValueDefined())
				{
					if (!isUInt)
					{
						const TypeDescriptor uintType = TypeDescriptor::GetUIntType();
						const Value length = CastValue(tav, &uintType);
						tav.SetValue(length);
					}

					if (tav.GetUIntValue() == 0)
					{
						mErrorBuffer.PushError(ParseError::ARRAY_SIZE_IS_ZERO, expressionList->GetContextToken());
					}
				}
				else
				{
					mErrorBuffer.PushError(ParseError::ARRAY_SIZE_IS_NOT_CONST_INTEGER, expressionList->GetContextToken());
					tav.SetUIntValue(1);
				}

				if (!isUInt)
				{
					expressionList->SetTypeDescriptor(TypeDescriptor::GetUIntType());
				}
			}
			expressionList = NextNode(expressionList);
		}
	}
}


void ValueEvaluationPass::Visit(NamedInitializer *namedInitializer)
{
	TypeAndValue &tav = *namedInitializer->GetTypeAndValue();
	if (!tav.IsResolved())
	{
		ParseNodeTraverser::Visit(namedInitializer);
		const TypeDescriptor *typeDescriptor = tav.GetTypeDescriptor();
		const Initializer *initializer = namedInitializer->GetInitializer();

		if (initializer == 0)
		{
			Resolve(tav);
		}
		else if (initializer->IsResolved() && typeDescriptor->IsResolved())
		{
			Resolve(tav);

			// TODO: Handle non-primitive types (e.g. arrays of primitive types or string literals).
			if ((typeDescriptor->GetPrimitiveType() != Token::INVALID) && typeDescriptor->IsConst())
			{
				const TypeAndValue initializerTav = initializer->GetExpression()->GetTypeAndValue();
				if (initializerTav.IsValueDefined())
				{
					tav.SetValue(CastValue(initializerTav, typeDescriptor));
				}
			}
		}
		CheckUnresolved(tav);
	}
}


void ValueEvaluationPass::Visit(SwitchLabel *switchLabel)
{
	ParseNodeTraverser::Visit(switchLabel);
	const Expression *expression = switchLabel->GetExpression();
	if (expression != 0)
	{
		const TypeAndValue &tav = expression->GetTypeAndValue();
		if (tav.IsResolved() && !tav.IsValueDefined())
		{
			mErrorBuffer.PushError(ParseError::SWITCH_LABEL_IS_NOT_CONST_INTEGER, expression->GetContextToken());
		}
		CheckUnresolved(tav);
	}
}


void ValueEvaluationPass::Visit(DeclarativeStatement *declarativeStatement)
{
	TypeDescriptor *typeDescriptor = declarativeStatement->GetTypeDescriptor();
	if ((typeDescriptor->IsArrayType()) &&
	    (CastNode<EmptyExpression>(typeDescriptor->GetLengthExpressionList()) != 0) &&
	    !typeDescriptor->GetLengthExpressionList()->GetTypeAndValue().IsValueDefined())
	{
		bu32_t length = 0;
		const NamedInitializer *current = declarativeStatement->GetNamedInitializerList();
		while (current != 0)
		{
			const Initializer *initializer = current->GetInitializer();
			if ((initializer != 0) && (initializer->GetInitializerList()) != 0)
			{
				const bu32_t initializerListLength = GetLength(initializer->GetInitializerList());
				length = (initializerListLength > length) ? initializerListLength : length;
			}
			current = NextNode(current);
		}

		if (length == 0)
		{
			mErrorBuffer.PushError(ParseError::ARRAY_SIZE_IS_UNSPECIFIED, typeDescriptor->GetContextToken(), typeDescriptor);
		}
		typeDescriptor->GetLengthExpressionList()->GetTypeAndValue().SetUIntValue(length);
	}
	ParseNodeTraverser::Visit(declarativeStatement);
}


void ValueEvaluationPass::Visit(ConditionalExpression *conditionalExpression)
{
	TypeAndValue &tav = conditionalExpression->GetTypeAndValue();
	if (!tav.IsResolved())
	{
		ParseNodeTraverser::Visit(conditionalExpression);
		const TypeAndValue &condTav = conditionalExpression->GetCondition()->GetTypeAndValue();
		const TypeAndValue &trueTav = conditionalExpression->GetTrueExpression()->GetTypeAndValue();
		const TypeAndValue &falseTav = conditionalExpression->GetFalseExpression()->GetTypeAndValue();

		if (condTav.IsResolved() && trueTav.IsResolved() && falseTav.IsResolved())
		{
			Resolve(tav);

			if (condTav.IsValueDefined() && trueTav.IsValueDefined() && falseTav.IsValueDefined())
			{
				const bool cond = condTav.GetBoolValue();
				const TypeDescriptor *resultType = tav.GetTypeDescriptor();
				tav.SetValue(CastValue(cond ? trueTav : falseTav, resultType));
			}
		}
		CheckUnresolved(tav);
	}
}


void ValueEvaluationPass::Visit(BinaryExpression *binaryExpression)
{
	TypeAndValue &tav = binaryExpression->GetTypeAndValue();
	if (!tav.IsResolved())
	{
		ParseNodeTraverser::Visit(binaryExpression);
		const TypeAndValue &lhs = binaryExpression->GetLhs()->GetTypeAndValue();
		const TypeAndValue &rhs = binaryExpression->GetRhs()->GetTypeAndValue();

		if (lhs.IsResolved() && rhs.IsResolved())
		{
			Resolve(tav);

			if (lhs.IsValueDefined() && rhs.IsValueDefined())
			{
				const Token *op = binaryExpression->GetOperator();
				const TypeDescriptor *resultType = tav.GetTypeDescriptor();

				switch (op->GetTokenType())
				{
					case Token::COMMA:
						tav.SetValue(rhs.GetValue());
						break;
					case Token::OP_PLUS:
						tav.SetValue(BinaryAdd(lhs, rhs, resultType));
						break;
					case Token::OP_MINUS:
						tav.SetValue(BinarySub(lhs, rhs, resultType));
						break;
					case Token::OP_STAR:
						tav.SetValue(BinaryMult(lhs, rhs, resultType));
						break;
					case Token::OP_DIV:
						tav.SetValue(BinaryDiv(lhs, rhs, resultType));
						break;
					case Token::OP_MOD:
						tav.SetValue(BinaryMod(lhs, rhs, resultType));
						break;
					case Token::OP_LEFT:
						tav.SetValue(BinaryLeft(lhs, rhs, resultType));
						break;
					case Token::OP_RIGHT:
						tav.SetValue(BinaryRight(lhs, rhs, resultType));
						break;
					case Token::OP_AMP:
						tav.SetValue(BinaryBitAnd(lhs, rhs, resultType));
						break;
					case Token::OP_BIT_OR:
						tav.SetValue(BinaryBitOr(lhs, rhs, resultType));
						break;
					case Token::OP_BIT_XOR:
						tav.SetValue(BinaryBitXOr(lhs, rhs, resultType));
						break;
					case Token::OP_LT:
						tav.SetValue(BinaryLT(lhs, rhs));
						break;
					case Token::OP_LTE:
						tav.SetValue(BinaryLTE(lhs, rhs));
						break;
					case Token::OP_GT:
						tav.SetValue(BinaryGT(lhs, rhs));
						break;
					case Token::OP_GTE:
						tav.SetValue(BinaryGTE(lhs, rhs));
						break;
					case Token::OP_EQUAL:
						tav.SetValue(BinaryEqual(lhs, rhs));
						break;
					case Token::OP_NOT_EQUAL:
						tav.SetValue(BinaryNotEqual(lhs, rhs));
						break;
					case Token::OP_AND:
						tav.SetBoolValue(lhs.GetBoolValue() && rhs.GetBoolValue());
						break;
					case Token::OP_OR:
						tav.SetBoolValue(lhs.GetBoolValue() || rhs.GetBoolValue());
						break;
					default:
						break;
				}
			}
		}
		CheckUnresolved(tav);
	}
}


void ValueEvaluationPass::Visit(UnaryExpression *unaryExpression)
{
	TypeAndValue &tav = unaryExpression->GetTypeAndValue();
	if (!tav.IsResolved())
	{
		ParseNodeTraverser::Visit(unaryExpression);
		const TypeAndValue &rhs = unaryExpression->GetRhs()->GetTypeAndValue();

		if (rhs.IsResolved())
		{
			Resolve(tav);

			if (rhs.IsValueDefined())
			{
				const Token *op = unaryExpression->GetOperator();

				switch (op->GetTokenType())
				{
					case Token::OP_PLUS:
						tav.SetValue(rhs.GetValue());
						break;
					case Token::OP_MINUS:
						tav.SetValue(UnaryMinus(rhs));
						break;
					case Token::OP_NOT:
						tav.SetBoolValue(!rhs.GetBoolValue());
						break;
					case Token::OP_BIT_NOT:
						tav.SetValue(UnaryBitNot(rhs));
						break;
					case Token::OP_STAR:
						// TODO
						break;
					default:
						break;
				}
			}
		}
		CheckUnresolved(tav);
	}
}


void ValueEvaluationPass::Visit(PostfixExpression *postfixExpression)
{
	// Value can't be evaluated at compile time. Mark as resolved when operand is resolved.
	TypeAndValue &tav = postfixExpression->GetTypeAndValue();
	if (!tav.IsResolved())
	{
		ParseNodeTraverser::Visit(postfixExpression);
		const TypeAndValue &lhs = postfixExpression->GetLhs()->GetTypeAndValue();
		if (lhs.IsResolved())
		{
			Resolve(tav);
		}
		CheckUnresolved(tav);
	}
}


void ValueEvaluationPass::Visit(MemberExpression *memberExpression)
{
	// Value can't be evaluated at compile time. Mark as resolved when operand is resolved.
	TypeAndValue &tav = memberExpression->GetTypeAndValue();
	if (!tav.IsResolved())
	{
		ParseNodeTraverser::Visit(memberExpression);
		const TypeAndValue &lhs = memberExpression->GetLhs()->GetTypeAndValue();
		if (lhs.IsResolved())
		{
			Resolve(tav);
		}
		CheckUnresolved(tav);
	}
}


void ValueEvaluationPass::Visit(ArraySubscriptExpression *arraySubscriptExpression)
{
	// TODO: A constant index into a constant array could be be evaluated at compile time.
	// Mark as resolved when array and index are resolved.
	TypeAndValue &tav = arraySubscriptExpression->GetTypeAndValue();
	if (!tav.IsResolved())
	{
		ParseNodeTraverser::Visit(arraySubscriptExpression);
		const TypeAndValue &lhs = arraySubscriptExpression->GetLhs()->GetTypeAndValue();
		const TypeAndValue &indexTav = arraySubscriptExpression->GetIndex()->GetTypeAndValue();
		if (lhs.IsResolved() && indexTav.IsResolved())
		{
			Resolve(tav);
		}
		CheckUnresolved(tav);
	}
}


void ValueEvaluationPass::Visit(FunctionCallExpression *functionCallExpression)
{
	// Value can't be evaluated at compile time. Mark as resolved when lhs and all arguments are resolved.
	TypeAndValue &tav = functionCallExpression->GetTypeAndValue();
	if (!tav.IsResolved())
	{
		ParseNodeTraverser::Visit(functionCallExpression);
		const TypeAndValue &lhs = functionCallExpression->GetLhs()->GetTypeAndValue();
		if (!lhs.IsResolved())
		{
			return;
		}

		const Expression *argument = functionCallExpression->GetArgumentList();
		while (argument != 0)
		{
			const TypeAndValue &argTav = argument->GetTypeAndValue();
			if (!argTav.IsResolved())
			{
				return;
			}
			argument = NextNode(argument);
		}
		Resolve(tav);
	}
}


void ValueEvaluationPass::Visit(CastExpression *castExpression)
{
	TypeAndValue &tav = castExpression->GetTypeAndValue();
	if (!tav.IsResolved())
	{
		ParseNodeTraverser::Visit(castExpression);
		const TypeAndValue &rhs = castExpression->GetRhs()->GetTypeAndValue();
		if (rhs.IsResolved())
		{
			Resolve(tav);
			const TypeDescriptor *resultType = tav.GetTypeDescriptor();
			tav.SetValue(CastValue(rhs, resultType));
		}
		CheckUnresolved(tav);
	}
}


void ValueEvaluationPass::Visit(SizeofExpression *sizeofExpression)
{
	TypeAndValue &tav = sizeofExpression->GetTypeAndValue();
	if (!tav.IsResolved())
	{
		ParseNodeTraverser::Visit(sizeofExpression);
		const TypeDescriptor *typeDescriptor = 0;

		if (sizeofExpression->GetRhs() != 0)
		{
			const TypeAndValue &rhs = sizeofExpression->GetRhs()->GetTypeAndValue();
			if (rhs.IsResolved())
			{
				typeDescriptor = rhs.GetTypeDescriptor();
			}
		}
		else
		{
			typeDescriptor = sizeofExpression->GetTypeDescriptor();
		}

		if ((typeDescriptor != 0) && typeDescriptor->IsResolved())
		{
			Resolve(tav);
			tav.SetUIntValue(typeDescriptor->GetSize(mPointerSize));
		}
		CheckUnresolved(tav);
	}
}


void ValueEvaluationPass::Visit(ConstantExpression *constantExpression)
{
	TypeAndValue &tav = constantExpression->GetTypeAndValue();
	if (!tav.IsResolved())
	{
		const Token *token = constantExpression->GetValueToken();
		Resolve(tav);
		tav.SetValue(token->GetValue());
	}
}


void ValueEvaluationPass::Visit(IdentifierExpression *identifierExpression)
{
	TypeAndValue &tav = identifierExpression->GetTypeAndValue();
	if (!tav.IsResolved())
	{
		const TypeAndValue &definitionTav = *identifierExpression->GetDefinition()->GetTypeAndValue();
		if (definitionTav.IsResolved())
		{
			Resolve(tav);
			if (definitionTav.IsValueDefined())
			{
				tav.SetValue(definitionTav.GetValue());
			}
		}
		else
		{
			mUnresolvedErrorBuffer.PushError(ParseError::CANNOT_RESOLVE_SYMBOL_VALUE, identifierExpression->GetContextToken());
		}
		CheckUnresolved(tav);
	}
}


void ValueEvaluationPass::Resolve(TypeAndValue &tav)
{
	tav.Resolve();
	mHasResolvedItems = true;
}


void ValueEvaluationPass::CheckUnresolved(const TypeAndValue &tav)
{
	mHasUnresolvedItems = mHasUnresolvedItems || !tav.IsResolved();
}


bool ValueEvaluationPass::AlignmentComparator::operator()(const DeclarativeStatement &a, const DeclarativeStatement &b) const
{
	const TypeDescriptor *aType = a.GetTypeDescriptor();
	const TypeDescriptor *bType = b.GetTypeDescriptor();
	const bu32_t aAlign = aType->GetAlignment();
	const bu32_t bAlign = bType->GetAlignment();
	return aAlign > bAlign;
}

}
