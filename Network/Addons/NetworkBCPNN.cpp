#include "NetworkBCPNN.h"

void ConnectionModifierBcpnnOnline::Modify()
{
	// Add mpimakelayerlocal ?
	if(IsOn() == false) return;

	int nodeId = m_connectionFixed->PreLayer()->network()->MPIGetNodeId();

	if(nodeId == 0) // will be put in mpi-class
	{
		cout<<".";
		cout.flush();
	}

	vector<float> postValues = m_connectionFixed->GetPostValues();
	vector<float> preValues;

	//vector<long> indexesPost = m_connectionFixed->GetPostLocalIds(); // only need to run once
	//vector<Unit*> postUnits = m_connectionFixed->
	long preId;
	float weight;
	float oldWeight;
	bool noUpdate = false;

	// here we assume all posts are connecting to the same pres, if not we should change it so that all possible pres should be calculated (whole layer below)

	//vector<float>* allPreValues = m_network->GetIncomingBufferData();//m_connectionFixed->GetPreValuesAll(); // = m_connectionFixed->m_idsPost[0]);//m_connectionFixed->GetPreValues(m_idsPost[0]);

	vector<float> allPreValues = m_connectionFixed->GetPreValuesAll();
	
	if(allPreValues.size()!=m_Ai.size()) // connections have been rebuilt, re-initialize variables (could also run initialize first simulate step)
	{
		this->Initialize(m_connectionFixed);
	}

	map<long,float>::iterator it1;

	// use union of all pre connections
	//for(int i=0;i<preValues.size();i++)//m_Ai.size;i++)
	int i=0;
	float preVal;
	for(it1 = m_Ai.begin(); it1!=m_Ai.end(); it1++)
	{
		preVal = allPreValues[i];//(*allPreValues)[it1->first];
		/*if(preVal == 0)
			m_Ai[it1->first] = m_Ai[it1->first] + m_alpha*(m_lambda0-m_Ai[it1->first]);
		else*/
		//m_Ai[i] = m_Ai[i] + m_alpha*(( (1-m_lambda0)*preValues[i] + m_lambda0)-m_Ai[i]);
			m_Ai[it1->first] = m_Ai[it1->first] + m_alpha*(( (1-m_lambda0)*preVal + m_lambda0)-m_Ai[it1->first]);
		i++;
	}

	float lambda0Sqr = m_lambda0*m_lambda0;

	for(int j=0;j<postValues.size();j++)//m_Aj.size;j++)
	{

		/*if(postValues[j] == 0)
			m_Aj[j] = m_Aj[j] + m_alpha*(m_lambda0-m_Aj[j]);
		else*/
			m_Aj[j] = m_Aj[j] + m_alpha*(( (1-m_lambda0)*postValues[j] + m_lambda0)-m_Aj[j]);

		if(m_impactBeta < 0.0 && m_impactWeights == 0.0) // special case, putting inhib thresh outside normalization
		{
			/*if(postValues[j]==1.0)
			{
				m_inhibBeta[j] = 0;//-= (exp(-m_inhibBeta[j])-1)*(-m_impactBeta) + 0.00001;//(-m_impactBeta);//(exp(-m_inhibBeta[j])-1)*(-m_impactBeta);//0;//+= exp(-m_inhibBeta[j])-1;//(exp(-m_inhibBeta[j])-1);//(-m_impactBeta)*(exp(-m_inhibBeta[j])-1);
				bool jj =false;
			}
			else
			{
				m_inhibBeta[j] += (-m_impactBeta); //+= (exp(m_inhibBeta[j])-1)*(-m_impactBeta) + 0.00001;//(-m_impactBeta);//(exp(m_inhibBeta[j])-1)*(-m_impactBeta);//-m_impactBeta;//(exp(m_inhibBeta[j])-1) + 0.00001;//-m_impactBeta;//(exp(m_inhibBeta[j])-1)/5 + (-m_impactBeta);//(-m_impactBeta);//(-m_impactBeta)*((exp(m_inhibBeta[j])-1) + 1);//(-m_impactBeta);
			}*/

			//float aim = 1.0/(float)preValues.size();
			float aim = 1.0/(float)m_connectionFixed->GetPreIdsAll()->size();

			m_inhibBeta[j] += (-m_impactBeta)*(aim - m_Aj[j]);//postValues[j]);//m_Aj[j]);

			if(m_inhibBeta[j]<aim) m_inhibBeta[j] = aim;
			//m_inhibBeta[j] = m_impactBeta*log(m_Aj[j]);
			//m_inhibBeta[j] = -(log(m_Aj[j])-log((float)1/(float)m_Ai.size()))/100;//-log(m_Aj[j]);

			((RateUnit*)m_connectionFixed->PostLayer()->network()->GetUnitFromId(m_postIds[j]))->AddInhibBeta(m_inhibBeta[j]);
		}
		else
		{
			m_beta[j] = m_impactBeta * log(m_Aj[j]);
			((RateUnit*)m_connectionFixed->PostLayer()->network()->GetUnitFromId(m_postIds[j]))->SetBeta(m_beta[j]);//AddBeta(m_beta[j]);
		}

		preValues = m_connectionFixed->GetPreValues(m_postIds[j]);

		if(m_impactWeights != 0.0)
		{
			vector<long> preIds = m_connectionFixed->GetPreIds(m_postIds[j]);
			//map<long, Network::SynapseStandard>* preSynapses = m_network->GetPreSynapses(m_postIds[j]); // will get all synapses originating from all different populations (!)
			//map<long, Network::SynapseStandard>::iterator it;
	
			int i = 0;
			float aij;

			for(int i = 0;i<preIds.size();i++)
//			for(it = preSynapses->begin(); it!=preSynapses->end(); it++) // slower than vector iterators
//			for(int i=0;i<preValues.size();i++)//m_Ai.size;i++)
			{
				preId = preIds[i];
				//preId = it->first;
				//aij = m_Aij[j][preId];
				aij = m_Aij[j][i];
				
				/*if(preValues[i] == 0 || postValues[j] == 0)
					m_Aij[j][preId] = aij + m_alpha*(lambda0Sqr - aij);
				else*/
					//m_Aij[j][preId] = aij + m_alpha*(((1-lambda0Sqr)*preValues[i]*postValues[j] + lambda0Sqr) - aij);
					m_Aij[j][i] = aij + m_alpha*(((1-lambda0Sqr)*preValues[i]*postValues[j] + lambda0Sqr) - aij);

				//weight = m_Aij[j][preId] / (m_Ai[preId]*m_Aj[j]);
				weight = m_Aij[j][i] / (m_Ai[preId]*m_Aj[j]);
				//m_Aij[i][j] = m_Aij[i][j] + m_alpha*(((1-m_lambda0*m_lambda0)*preValues[i]*postValues[j] + m_lambda0*m_lambda0) - m_Aij[i][j]);
				//weight = m_Aij[i][j] / (m_Ai[i]*m_Aj[j]);

				//vector<vector<long> >* preIds = m_connectionFixed->PreIds();
				//preId = (*preIds)[j][i];

				if(m_useNoUpdateIfNoPreActivity)
				{
					if(m_Aj[j] == m_lambda0)
						noUpdate = true;
					else noUpdate = false;
				}

				//oldWeight = network()->GetWeight(preId,m_idsPost[j]);
				//weight = oldWeight + m_alpha*(weight-oldWeight);

				if(noUpdate == false)
				{
					if(preId != m_postIds[j])
						//					it->second = m_impactWeights * weight;			
						network()->SetWeight( m_impactWeights * weight,preId,m_postIds[j]);
					else
						network()->SetWeight( 0.0 ,preId,m_postIds[j]);
				}
				//i++;
			}
		}
	}
}

void ConnectionModifierBcpnnOnline::Simulate(UnitModifier* e)
{
//	float weight = ((ConnectionFixed*)m_connectionFixed)->GetWeight();
//	float value = e->GetValue(); 
//	e->SetValue(weight*value);
}

void ConnectionModifierBcpnnOnline::Initialize(Connection* connection)
{
	network(connection->network());
	network()->SetTrackingHypercolumnIds(true);

	m_connectionFixed = connection;
	//vector<float> preValues = vector<float>(m_connectionFixed->PreLayer()->GetNrUnits(),0.0);//m_connectionFixed->GetPreValues();
	vector<float> postValues = m_connectionFixed->GetPostValues();
	m_postIds = m_connectionFixed->GetPostIds();
	m_Aj = vector<float>(m_postIds.size(),m_lambda0);
	//for(int i=0;i<m_Aj.size();i++)
	//	m_Aj[i] = 0.02*(float)rand()/(float)RAND_MAX;

	// get union of all pre ids
	vector<long>* preIdsUnion = m_connectionFixed->GetPreIdsAll();

	//m_Ai = vector<float>(preIdsUnion->size(),0.01);//= m_Aj = 0.001;
	m_Aij = vector<vector<float> >(m_postIds.size());//vector<map<long,float> >(m_postIds.size());//vector<vector<float> >(preValues.size(),vector<float>(postValues.size(),0.01*0.01));//0.001*0.001;
	m_Ai.clear();

	for(int i=0;i<preIdsUnion->size();i++)
		m_Ai[(*preIdsUnion)[i]] = m_lambda0;//0.02*(float)rand()/(float)RAND_MAX;//0.01;

	for(int i=0;i<m_postIds.size();i++)
	{
		/*map<long, Network::SynapseStandard>* preSynapses = m_network->GetPreSynapses(m_postIds[i]);
		map<long, Network::SynapseStandard>::iterator it;
		
		for(it=preSynapses->begin();it!=preSynapses->end();it++)
			m_Aij[i][it->first] = m_lambda0*0.1;//m_lambda0*m_lambda0;//0.02*(float)rand()/(float)RAND_MAX*0.02*(float)rand()/(float)RAND_MAX;//0.01*0.01;
			*/
		vector<long> preIds = m_connectionFixed->GetPreIds(m_postIds[i]);
		
		vector<float> f(preIds.size(),m_lambda0*m_lambda0);//m_lambda0*0.1);
		m_Aij[i] = f;
	}

	m_beta = vector<float>(postValues.size(),0);//0;
	if(m_impactBeta < 0.0 && m_impactWeights == 0.0)
		m_inhibBeta = vector<float>(postValues.size(),0.0);

	m_firstRun = true;
	m_postIds = m_connectionFixed->GetPostIds();
}

ConnectionModifierBcpnnOnline::ConnectionModifierBcpnnOnline(float alpha, float lambda, float maxValue)
{
	m_impactWeights = 1.0;
	m_impactBeta = 1.0;
	m_alpha = alpha;
	m_lambda0 = lambda;
	m_useNoUpdateIfNoPreActivity = false;

	m_eventId = 1;

	m_transferFunction = new TransferBcpnnOnline(maxValue);
	//ConnectionModifierBcpnnOnline();
}

void ConnectionModifierBcpnnOnline::SetAlpha(float alpha)
{
	m_alpha = alpha;
}

void ConnectionModifierBcpnnOnline::SetLambda(float lambda)
{
	m_lambda0 = lambda;
}

ConnectionModifierBcpnnOnline::ConnectionModifierBcpnnOnline()
{
	m_impactWeights = 1.0;
	m_impactBeta = 1.0;
	m_alpha = 0.01;
	m_lambda0 = 0.0001;
	m_eventId = 1;
	m_useNoUpdateIfNoPreActivity = false;

	m_transferFunction = new TransferBcpnnOnline();
}

void ConnectionModifierBcpnnOnline::SetConnection(Connection* c)
{
	m_connectionFixed = (ConnectionFixed*)c;
/*	m_pre = (RateUnit*)m_connectionFixed->Pre();
	m_post = (RateUnit*)m_connectionFixed->Post();
	*/
}