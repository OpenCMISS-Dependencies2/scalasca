#include <string.h>

#include <string>
#include <vector>
#include <map>

#include <otf2/otf2.h>


class Otf2Definitions
{
public:
    Otf2Definitions( OTF2_Reader* reader );
    ~Otf2Definitions();

    typedef std::map< OTF2_StringRef, std::string > StringT;
    typedef StringT::iterator                       StringIT;
    typedef StringT::const_iterator                 StringCIT;
    typedef StringT::value_type                     StringV;
    StringT m_Strings;


    struct ClockProperties
    {
        uint64_t timerResolution;
        uint64_t globalOffset;
        uint64_t traceLength;
    };
    typedef std::vector< ClockProperties* >  ClockPropertiesT;
    typedef ClockPropertiesT::iterator       ClockPropertiesIT;
    typedef ClockPropertiesT::const_iterator ClockPropertiesCIT;
    typedef ClockPropertiesT::value_type     ClockPropertiesV;
    ClockPropertiesT m_ClockProperties;

    struct Paradigm
    {
        OTF2_Paradigm      paradigm;
        std::string        name;
        OTF2_ParadigmClass paradigmClass;
    };
    typedef std::vector< Paradigm* >  ParadigmT;
    typedef ParadigmT::iterator       ParadigmIT;
    typedef ParadigmT::const_iterator ParadigmCIT;
    typedef ParadigmT::value_type     ParadigmV;
    ParadigmT m_Paradigms;

    struct ParadigmProperty
    {
        OTF2_Paradigm         paradigm;
        OTF2_ParadigmProperty property;
        OTF2_Type             type;
        OTF2_AttributeValue   value;
    };
    typedef std::vector< ParadigmProperty* >  ParadigmPropertyT;
    typedef ParadigmPropertyT::iterator       ParadigmPropertyIT;
    typedef ParadigmPropertyT::const_iterator ParadigmPropertyCIT;
    typedef ParadigmPropertyT::value_type     ParadigmPropertyV;
    ParadigmPropertyT m_ParadigmProperties;


    struct MetricClassRecorder
    {
        OTF2_LocationRef recorder;
    };
    typedef std::vector< MetricClassRecorder* >  MetricClassRecorderT;
    typedef MetricClassRecorderT::iterator       MetricClassRecorderIT;
    typedef MetricClassRecorderT::const_iterator MetricClassRecorderCIT;
    typedef MetricClassRecorderT::value_type     MetricClassRecorderV;

    struct SystemTreeNodeProperty
    {
        std::string         name;
        OTF2_Type           type;
        OTF2_AttributeValue value;
    };
    typedef std::vector< SystemTreeNodeProperty* >  SystemTreeNodePropertyT;
    typedef SystemTreeNodePropertyT::iterator       SystemTreeNodePropertyIT;
    typedef SystemTreeNodePropertyT::const_iterator SystemTreeNodePropertyCIT;
    typedef SystemTreeNodePropertyT::value_type     SystemTreeNodePropertyV;

    struct SystemTreeNodeDomain
    {
        OTF2_SystemTreeDomain systemTreeDomain;
    };
    typedef std::vector< SystemTreeNodeDomain* >  SystemTreeNodeDomainT;
    typedef SystemTreeNodeDomainT::iterator       SystemTreeNodeDomainIT;
    typedef SystemTreeNodeDomainT::const_iterator SystemTreeNodeDomainCIT;
    typedef SystemTreeNodeDomainT::value_type     SystemTreeNodeDomainV;

    struct LocationGroupProperty
    {
        std::string         name;
        OTF2_Type           type;
        OTF2_AttributeValue value;
    };
    typedef std::vector< LocationGroupProperty* >  LocationGroupPropertyT;
    typedef LocationGroupPropertyT::iterator       LocationGroupPropertyIT;
    typedef LocationGroupPropertyT::const_iterator LocationGroupPropertyCIT;
    typedef LocationGroupPropertyT::value_type     LocationGroupPropertyV;

    struct LocationProperty
    {
        std::string         name;
        OTF2_Type           type;
        OTF2_AttributeValue value;
    };
    typedef std::vector< LocationProperty* >  LocationPropertyT;
    typedef LocationPropertyT::iterator       LocationPropertyIT;
    typedef LocationPropertyT::const_iterator LocationPropertyCIT;
    typedef LocationPropertyT::value_type     LocationPropertyV;

    struct CartCoordinate
    {
        uint32_t                rank;
        std::vector< uint32_t > coordinates;
    };
    typedef std::vector< CartCoordinate* >  CartCoordinateT;
    typedef CartCoordinateT::iterator       CartCoordinateIT;
    typedef CartCoordinateT::const_iterator CartCoordinateCIT;
    typedef CartCoordinateT::value_type     CartCoordinateV;

    struct CallingContextProperty
    {
        std::string         name;
        OTF2_Type           type;
        OTF2_AttributeValue value;
    };
    typedef std::vector< CallingContextProperty* >  CallingContextPropertyT;
    typedef CallingContextPropertyT::iterator       CallingContextPropertyIT;
    typedef CallingContextPropertyT::const_iterator CallingContextPropertyCIT;
    typedef CallingContextPropertyT::value_type     CallingContextPropertyV;

    struct IoFileProperty
    {
        std::string         name;
        OTF2_Type           type;
        OTF2_AttributeValue value;
    };
    typedef std::vector< IoFileProperty* >  IoFilePropertyT;
    typedef IoFilePropertyT::iterator       IoFilePropertyIT;
    typedef IoFilePropertyT::const_iterator IoFilePropertyCIT;
    typedef IoFilePropertyT::value_type     IoFilePropertyV;

    struct IoPreCreatedHandleState
    {
        OTF2_IoAccessMode mode;
        OTF2_IoStatusFlag statusFlags;
    };
    typedef std::vector< IoPreCreatedHandleState* >  IoPreCreatedHandleStateT;
    typedef IoPreCreatedHandleStateT::iterator       IoPreCreatedHandleStateIT;
    typedef IoPreCreatedHandleStateT::const_iterator IoPreCreatedHandleStateCIT;
    typedef IoPreCreatedHandleStateT::value_type     IoPreCreatedHandleStateV;

    struct CallpathParameter
    {
        OTF2_ParameterRef   parameter;
        OTF2_Type           type;
        OTF2_AttributeValue value;
    };
    typedef std::vector< CallpathParameter* >  CallpathParameterT;
    typedef CallpathParameterT::iterator       CallpathParameterIT;
    typedef CallpathParameterT::const_iterator CallpathParameterCIT;
    typedef CallpathParameterT::value_type     CallpathParameterV;

    enum MetricType
    {
        METRIC_CLASS,
        METRIC_INSTANCE,
    };
    struct Metric
    {
        MetricType           metricType;
        MetricClassRecorderT metricClassRecorders;
    };
    typedef std::map< OTF2_MetricRef, Metric* > MetricT;
    typedef MetricT::iterator                   MetricIT;
    typedef MetricT::const_iterator             MetricCIT;
    typedef MetricT::value_type                 MetricV;
    MetricT m_Metrics;

    enum IoFileType
    {
        IO_REGULAR_FILE,
        IO_DIRECTORY,
    };
    struct IoFile
    {
        IoFileType      ioFileType;
        IoFilePropertyT ioFileProperties;
    };
    typedef std::map< OTF2_IoFileRef, IoFile* > IoFileT;
    typedef IoFileT::iterator                   IoFileIT;
    typedef IoFileT::const_iterator             IoFileCIT;
    typedef IoFileT::value_type                 IoFileV;
    IoFileT m_IoFiles;



    struct IoParadigm
    {
        std::string                            identification;
        std::string                            name;
        OTF2_IoParadigmClass                   ioParadigmClass;
        OTF2_IoParadigmFlag                    ioParadigmFlags;
        std::vector< OTF2_IoParadigmProperty > properties;
        std::vector< OTF2_Type >               types;
        std::vector< OTF2_AttributeValue >     values;
    };
    typedef std::map< OTF2_IoParadigmRef, IoParadigm* > IoParadigmT;
    typedef IoParadigmT::iterator                       IoParadigmIT;
    typedef IoParadigmT::const_iterator                 IoParadigmCIT;
    typedef IoParadigmT::value_type                     IoParadigmV;
    IoParadigmT m_IoParadigms;

    struct Attribute
    {
        std::string name;
        std::string description;
        OTF2_Type   type;
    };
    typedef std::map< OTF2_AttributeRef, Attribute* > AttributeT;
    typedef AttributeT::iterator                      AttributeIT;
    typedef AttributeT::const_iterator                AttributeCIT;
    typedef AttributeT::value_type                    AttributeV;
    AttributeT m_Attributes;

    struct SystemTreeNode
    {
        std::string             name;
        std::string             className;
        OTF2_SystemTreeNodeRef  parent;
        SystemTreeNodePropertyT systemTreeNodeProperties;
        SystemTreeNodeDomainT   systemTreeNodeDomains;
    };
    typedef std::map< OTF2_SystemTreeNodeRef, SystemTreeNode* > SystemTreeNodeT;
    typedef SystemTreeNodeT::iterator                           SystemTreeNodeIT;
    typedef SystemTreeNodeT::const_iterator                     SystemTreeNodeCIT;
    typedef SystemTreeNodeT::value_type                         SystemTreeNodeV;
    SystemTreeNodeT m_SystemTreeNodes;

    struct LocationGroup
    {
        std::string            name;
        OTF2_LocationGroupType locationGroupType;
        OTF2_SystemTreeNodeRef systemTreeParent;
        LocationGroupPropertyT locationGroupProperties;
    };
    typedef std::map< OTF2_LocationGroupRef, LocationGroup* > LocationGroupT;
    typedef LocationGroupT::iterator                          LocationGroupIT;
    typedef LocationGroupT::const_iterator                    LocationGroupCIT;
    typedef LocationGroupT::value_type                        LocationGroupV;
    LocationGroupT m_LocationGroups;

    struct Location
    {
        std::string           name;
        OTF2_LocationType     locationType;
        uint64_t              numberOfEvents;
        OTF2_LocationGroupRef locationGroup;
        LocationPropertyT     locationProperties;
    };
    typedef std::map< OTF2_LocationRef, Location* > LocationT;
    typedef LocationT::iterator                     LocationIT;
    typedef LocationT::const_iterator               LocationCIT;
    typedef LocationT::value_type                   LocationV;
    LocationT m_Locations;

    struct Region
    {
        std::string     name;
        std::string     canonicalName;
        std::string     description;
        OTF2_RegionRole regionRole;
        OTF2_Paradigm   paradigm;
        OTF2_RegionFlag regionFlags;
        std::string     sourceFile;
        uint32_t        beginLineNumber;
        uint32_t        endLineNumber;
    };
    typedef std::map< OTF2_RegionRef, Region* > RegionT;
    typedef RegionT::iterator                   RegionIT;
    typedef RegionT::const_iterator             RegionCIT;
    typedef RegionT::value_type                 RegionV;
    RegionT m_Regions;

    struct Callsite
    {
        std::string    sourceFile;
        uint32_t       lineNumber;
        OTF2_RegionRef enteredRegion;
        OTF2_RegionRef leftRegion;
    };
    typedef std::map< OTF2_CallsiteRef, Callsite* > CallsiteT;
    typedef CallsiteT::iterator                     CallsiteIT;
    typedef CallsiteT::const_iterator               CallsiteCIT;
    typedef CallsiteT::value_type                   CallsiteV;
    CallsiteT m_Callsites;

    struct Callpath
    {
        OTF2_CallpathRef   parent;
        OTF2_RegionRef     region;
        CallpathParameterT callpathParameters;
    };
    typedef std::map< OTF2_CallpathRef, Callpath* > CallpathT;
    typedef CallpathT::iterator                     CallpathIT;
    typedef CallpathT::const_iterator               CallpathCIT;
    typedef CallpathT::value_type                   CallpathV;
    CallpathT m_Callpaths;

    struct Group
    {
        std::string             name;
        OTF2_GroupType          groupType;
        OTF2_Paradigm           paradigm;
        OTF2_GroupFlag          groupFlags;
        std::vector< uint64_t > members;
    };
    typedef std::map< OTF2_GroupRef, Group* > GroupT;
    typedef GroupT::iterator                  GroupIT;
    typedef GroupT::const_iterator            GroupCIT;
    typedef GroupT::value_type                GroupV;
    GroupT m_Groups;

    struct MetricMember
    {
        std::string     name;
        std::string     description;
        OTF2_MetricType metricType;
        OTF2_MetricMode metricMode;
        OTF2_Type       valueType;
        OTF2_Base       base;
        int64_t         exponent;
        std::string     unit;
    };
    typedef std::map< OTF2_MetricMemberRef, MetricMember* > MetricMemberT;
    typedef MetricMemberT::iterator                         MetricMemberIT;
    typedef MetricMemberT::const_iterator                   MetricMemberCIT;
    typedef MetricMemberT::value_type                       MetricMemberV;
    MetricMemberT m_MetricMembers;

    struct MetricClass
        : public Metric
    {
        std::vector< OTF2_MetricMemberRef > metricMembers;
        OTF2_MetricOccurrence               metricOccurrence;
        OTF2_RecorderKind                   recorderKind;
    };

    struct MetricInstance
        : public Metric
    {
        OTF2_MetricRef   metricClass;
        OTF2_LocationRef recorder;
        OTF2_MetricScope metricScope;
        uint64_t         scope;
    };

    struct Comm
    {
        std::string   name;
        OTF2_GroupRef group;
        OTF2_CommRef  parent;
    };
    typedef std::map< OTF2_CommRef, Comm* > CommT;
    typedef CommT::iterator                 CommIT;
    typedef CommT::const_iterator           CommCIT;
    typedef CommT::value_type               CommV;
    CommT m_Comms;

    struct Parameter
    {
        std::string        name;
        OTF2_ParameterType parameterType;
    };
    typedef std::map< OTF2_ParameterRef, Parameter* > ParameterT;
    typedef ParameterT::iterator                      ParameterIT;
    typedef ParameterT::const_iterator                ParameterCIT;
    typedef ParameterT::value_type                    ParameterV;
    ParameterT m_Parameters;

    struct RmaWin
    {
        std::string  name;
        OTF2_CommRef comm;
    };
    typedef std::map< OTF2_RmaWinRef, RmaWin* > RmaWinT;
    typedef RmaWinT::iterator                   RmaWinIT;
    typedef RmaWinT::const_iterator             RmaWinCIT;
    typedef RmaWinT::value_type                 RmaWinV;
    RmaWinT m_RmaWins;

    struct CartDimension
    {
        std::string          name;
        uint32_t             size;
        OTF2_CartPeriodicity cartPeriodicity;
    };
    typedef std::map< OTF2_CartDimensionRef, CartDimension* > CartDimensionT;
    typedef CartDimensionT::iterator                          CartDimensionIT;
    typedef CartDimensionT::const_iterator                    CartDimensionCIT;
    typedef CartDimensionT::value_type                        CartDimensionV;
    CartDimensionT m_CartDimensions;

    struct CartTopology
    {
        std::string                          name;
        OTF2_CommRef                         communicator;
        std::vector< OTF2_CartDimensionRef > cartDimensions;
        CartCoordinateT                      cartCoordinates;
    };
    typedef std::map< OTF2_CartTopologyRef, CartTopology* > CartTopologyT;
    typedef CartTopologyT::iterator                         CartTopologyIT;
    typedef CartTopologyT::const_iterator                   CartTopologyCIT;
    typedef CartTopologyT::value_type                       CartTopologyV;
    CartTopologyT m_CartTopologies;

    struct SourceCodeLocation
    {
        std::string file;
        uint32_t    lineNumber;
    };
    typedef std::map< OTF2_SourceCodeLocationRef, SourceCodeLocation* > SourceCodeLocationT;
    typedef SourceCodeLocationT::iterator                               SourceCodeLocationIT;
    typedef SourceCodeLocationT::const_iterator                         SourceCodeLocationCIT;
    typedef SourceCodeLocationT::value_type                             SourceCodeLocationV;
    SourceCodeLocationT m_SourceCodeLocations;

    struct CallingContext
    {
        OTF2_RegionRef             region;
        OTF2_SourceCodeLocationRef sourceCodeLocation;
        OTF2_CallingContextRef     parent;
        CallingContextPropertyT    callingContextProperties;
    };
    typedef std::map< OTF2_CallingContextRef, CallingContext* > CallingContextT;
    typedef CallingContextT::iterator                           CallingContextIT;
    typedef CallingContextT::const_iterator                     CallingContextCIT;
    typedef CallingContextT::value_type                         CallingContextV;
    CallingContextT m_CallingContexts;

    struct InterruptGenerator
    {
        std::string                 name;
        OTF2_InterruptGeneratorMode interruptGeneratorMode;
        OTF2_Base                   base;
        int64_t                     exponent;
        uint64_t                    period;
    };
    typedef std::map< OTF2_InterruptGeneratorRef, InterruptGenerator* > InterruptGeneratorT;
    typedef InterruptGeneratorT::iterator                               InterruptGeneratorIT;
    typedef InterruptGeneratorT::const_iterator                         InterruptGeneratorCIT;
    typedef InterruptGeneratorT::value_type                             InterruptGeneratorV;
    InterruptGeneratorT m_InterruptGenerators;

    struct IoRegularFile
        : public IoFile
    {
        std::string            name;
        OTF2_SystemTreeNodeRef scope;
    };

    struct IoDirectory
        : public IoFile
    {
        std::string            name;
        OTF2_SystemTreeNodeRef scope;
    };

    struct IoHandle
    {
        std::string              name;
        OTF2_IoFileRef           file;
        OTF2_IoParadigmRef       ioParadigm;
        OTF2_IoHandleFlag        ioHandleFlags;
        OTF2_CommRef             comm;
        OTF2_IoHandleRef         parent;
        IoPreCreatedHandleStateT ioPreCreatedHandleStates;
    };
    typedef std::map< OTF2_IoHandleRef, IoHandle* > IoHandleT;
    typedef IoHandleT::iterator                     IoHandleIT;
    typedef IoHandleT::const_iterator               IoHandleCIT;
    typedef IoHandleT::value_type                   IoHandleV;
    IoHandleT m_IoHandles;

    OTF2_CallbackCode
    HandleDefClockProperties( uint64_t timerResolution,
                              uint64_t globalOffset,
                              uint64_t traceLength );

    OTF2_CallbackCode
    HandleDefParadigm( OTF2_Paradigm      paradigm,
                       OTF2_StringRef     name,
                       OTF2_ParadigmClass paradigmClass );

    OTF2_CallbackCode
    HandleDefParadigmProperty( OTF2_Paradigm         paradigm,
                               OTF2_ParadigmProperty property,
                               OTF2_Type             type,
                               OTF2_AttributeValue   value );

    OTF2_CallbackCode
    HandleDefIoParadigm( OTF2_IoParadigmRef             self,
                         OTF2_StringRef                 identification,
                         OTF2_StringRef                 name,
                         OTF2_IoParadigmClass           ioParadigmClass,
                         OTF2_IoParadigmFlag            ioParadigmFlags,
                         uint8_t                        numberOfProperties,
                         const OTF2_IoParadigmProperty* properties,
                         const OTF2_Type*               types,
                         const OTF2_AttributeValue*     values );

    OTF2_CallbackCode
    HandleDefString( OTF2_StringRef self,
                     const char*    string );

    OTF2_CallbackCode
    HandleDefAttribute( OTF2_AttributeRef self,
                        OTF2_StringRef    name,
                        OTF2_StringRef    description,
                        OTF2_Type         type );

    OTF2_CallbackCode
    HandleDefSystemTreeNode( OTF2_SystemTreeNodeRef self,
                             OTF2_StringRef         name,
                             OTF2_StringRef         className,
                             OTF2_SystemTreeNodeRef parent );

    OTF2_CallbackCode
    HandleDefLocationGroup( OTF2_LocationGroupRef  self,
                            OTF2_StringRef         name,
                            OTF2_LocationGroupType locationGroupType,
                            OTF2_SystemTreeNodeRef systemTreeParent );

    OTF2_CallbackCode
    HandleDefLocation( OTF2_LocationRef      self,
                       OTF2_StringRef        name,
                       OTF2_LocationType     locationType,
                       uint64_t              numberOfEvents,
                       OTF2_LocationGroupRef locationGroup );

    OTF2_CallbackCode
    HandleDefRegion( OTF2_RegionRef  self,
                     OTF2_StringRef  name,
                     OTF2_StringRef  canonicalName,
                     OTF2_StringRef  description,
                     OTF2_RegionRole regionRole,
                     OTF2_Paradigm   paradigm,
                     OTF2_RegionFlag regionFlags,
                     OTF2_StringRef  sourceFile,
                     uint32_t        beginLineNumber,
                     uint32_t        endLineNumber );

    OTF2_CallbackCode
    HandleDefCallsite( OTF2_CallsiteRef self,
                       OTF2_StringRef   sourceFile,
                       uint32_t         lineNumber,
                       OTF2_RegionRef   enteredRegion,
                       OTF2_RegionRef   leftRegion );

    OTF2_CallbackCode
    HandleDefCallpath( OTF2_CallpathRef self,
                       OTF2_CallpathRef parent,
                       OTF2_RegionRef   region );

    OTF2_CallbackCode
    HandleDefGroup( OTF2_GroupRef   self,
                    OTF2_StringRef  name,
                    OTF2_GroupType  groupType,
                    OTF2_Paradigm   paradigm,
                    OTF2_GroupFlag  groupFlags,
                    uint32_t        numberOfMembers,
                    const uint64_t* members );

    OTF2_CallbackCode
    HandleDefMetricMember( OTF2_MetricMemberRef self,
                           OTF2_StringRef       name,
                           OTF2_StringRef       description,
                           OTF2_MetricType      metricType,
                           OTF2_MetricMode      metricMode,
                           OTF2_Type            valueType,
                           OTF2_Base            base,
                           int64_t              exponent,
                           OTF2_StringRef       unit );

    OTF2_CallbackCode
    HandleDefMetricClass( OTF2_MetricRef              self,
                          uint8_t                     numberOfMetrics,
                          const OTF2_MetricMemberRef* metricMembers,
                          OTF2_MetricOccurrence       metricOccurrence,
                          OTF2_RecorderKind           recorderKind );

    OTF2_CallbackCode
    HandleDefMetricInstance( OTF2_MetricRef   self,
                             OTF2_MetricRef   metricClass,
                             OTF2_LocationRef recorder,
                             OTF2_MetricScope metricScope,
                             uint64_t         scope );

    OTF2_CallbackCode
    HandleDefComm( OTF2_CommRef   self,
                   OTF2_StringRef name,
                   OTF2_GroupRef  group,
                   OTF2_CommRef   parent );

    OTF2_CallbackCode
    HandleDefParameter( OTF2_ParameterRef  self,
                        OTF2_StringRef     name,
                        OTF2_ParameterType parameterType );

    OTF2_CallbackCode
    HandleDefRmaWin( OTF2_RmaWinRef self,
                     OTF2_StringRef name,
                     OTF2_CommRef   comm );

    OTF2_CallbackCode
    HandleDefMetricClassRecorder( OTF2_MetricRef   metric,
                                  OTF2_LocationRef recorder );

    OTF2_CallbackCode
    HandleDefSystemTreeNodeProperty( OTF2_SystemTreeNodeRef systemTreeNode,
                                     OTF2_StringRef         name,
                                     OTF2_Type              type,
                                     OTF2_AttributeValue    value );

    OTF2_CallbackCode
    HandleDefSystemTreeNodeDomain( OTF2_SystemTreeNodeRef systemTreeNode,
                                   OTF2_SystemTreeDomain  systemTreeDomain );

    OTF2_CallbackCode
    HandleDefLocationGroupProperty( OTF2_LocationGroupRef locationGroup,
                                    OTF2_StringRef        name,
                                    OTF2_Type             type,
                                    OTF2_AttributeValue   value );

    OTF2_CallbackCode
    HandleDefLocationProperty( OTF2_LocationRef    location,
                               OTF2_StringRef      name,
                               OTF2_Type           type,
                               OTF2_AttributeValue value );

    OTF2_CallbackCode
    HandleDefCartDimension( OTF2_CartDimensionRef self,
                            OTF2_StringRef        name,
                            uint32_t              size,
                            OTF2_CartPeriodicity  cartPeriodicity );

    OTF2_CallbackCode
    HandleDefCartTopology( OTF2_CartTopologyRef         self,
                           OTF2_StringRef               name,
                           OTF2_CommRef                 communicator,
                           uint8_t                      numberOfDimensions,
                           const OTF2_CartDimensionRef* cartDimensions );

    OTF2_CallbackCode
    HandleDefCartCoordinate( OTF2_CartTopologyRef cartTopology,
                             uint32_t             rank,
                             uint8_t              numberOfDimensions,
                             const uint32_t*      coordinates );

    OTF2_CallbackCode
    HandleDefSourceCodeLocation( OTF2_SourceCodeLocationRef self,
                                 OTF2_StringRef             file,
                                 uint32_t                   lineNumber );

    OTF2_CallbackCode
    HandleDefCallingContext( OTF2_CallingContextRef     self,
                             OTF2_RegionRef             region,
                             OTF2_SourceCodeLocationRef sourceCodeLocation,
                             OTF2_CallingContextRef     parent );

    OTF2_CallbackCode
    HandleDefCallingContextProperty( OTF2_CallingContextRef callingContext,
                                     OTF2_StringRef         name,
                                     OTF2_Type              type,
                                     OTF2_AttributeValue    value );

    OTF2_CallbackCode
    HandleDefInterruptGenerator( OTF2_InterruptGeneratorRef  self,
                                 OTF2_StringRef              name,
                                 OTF2_InterruptGeneratorMode interruptGeneratorMode,
                                 OTF2_Base                   base,
                                 int64_t                     exponent,
                                 uint64_t                    period );

    OTF2_CallbackCode
    HandleDefIoFileProperty( OTF2_IoFileRef      ioFile,
                             OTF2_StringRef      name,
                             OTF2_Type           type,
                             OTF2_AttributeValue value );

    OTF2_CallbackCode
    HandleDefIoRegularFile( OTF2_IoFileRef         self,
                            OTF2_StringRef         name,
                            OTF2_SystemTreeNodeRef scope );

    OTF2_CallbackCode
    HandleDefIoDirectory( OTF2_IoFileRef         self,
                          OTF2_StringRef         name,
                          OTF2_SystemTreeNodeRef scope );

    OTF2_CallbackCode
    HandleDefIoHandle( OTF2_IoHandleRef   self,
                       OTF2_StringRef     name,
                       OTF2_IoFileRef     file,
                       OTF2_IoParadigmRef ioParadigm,
                       OTF2_IoHandleFlag  ioHandleFlags,
                       OTF2_CommRef       comm,
                       OTF2_IoHandleRef   parent );

    OTF2_CallbackCode
    HandleDefIoPreCreatedHandleState( OTF2_IoHandleRef  ioHandle,
                                      OTF2_IoAccessMode mode,
                                      OTF2_IoStatusFlag statusFlags );

    OTF2_CallbackCode
    HandleDefCallpathParameter( OTF2_CallpathRef    callpath,
                                OTF2_ParameterRef   parameter,
                                OTF2_Type           type,
                                OTF2_AttributeValue value );

private:
    OTF2_Reader* m_Reader;
};

OTF2_CallbackCode
Otf2Definitions::HandleDefString( OTF2_StringRef stringID,
                                  const char*    string )
{
    m_Strings[ stringID ] = std::string( string );

    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
Otf2Definitions::HandleDefClockProperties( uint64_t timerResolution,
                                           uint64_t globalOffset,
                                           uint64_t traceLength )
{
    ClockProperties& new_clock_properties = *( new ClockProperties );

    new_clock_properties.timerResolution = timerResolution;
    new_clock_properties.globalOffset    = globalOffset;
    new_clock_properties.traceLength     = traceLength;

    m_ClockProperties.push_back( &new_clock_properties );

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefParadigm( OTF2_Paradigm      paradigm,
                                    OTF2_StringRef     name,
                                    OTF2_ParadigmClass paradigmClass )
{
    Paradigm& new_paradigm = *( new Paradigm );

    new_paradigm.paradigm      = paradigm;
    new_paradigm.name          = m_Strings[ name ];
    new_paradigm.paradigmClass = paradigmClass;

    m_Paradigms.push_back( &new_paradigm );

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefParadigmProperty( OTF2_Paradigm         paradigm,
                                            OTF2_ParadigmProperty property,
                                            OTF2_Type             type,
                                            OTF2_AttributeValue   value )
{
    ParadigmProperty& new_paradigm_property = *( new ParadigmProperty );

    new_paradigm_property.paradigm = paradigm;
    new_paradigm_property.property = property;
    new_paradigm_property.type     = type;
    new_paradigm_property.value    = value;

    m_ParadigmProperties.push_back( &new_paradigm_property );

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefMetricClassRecorder( OTF2_MetricRef   metric,
                                               OTF2_LocationRef recorder )
{
    MetricIT             parent_it                 = m_Metrics.find( metric );
    MetricClassRecorder& new_metric_class_recorder = *( new MetricClassRecorder );

    new_metric_class_recorder.recorder = recorder;

    parent_it->second->metricClassRecorders.push_back( &new_metric_class_recorder );

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefSystemTreeNodeProperty( OTF2_SystemTreeNodeRef systemTreeNode,
                                                  OTF2_StringRef         name,
                                                  OTF2_Type              type,
                                                  OTF2_AttributeValue    value )
{
    SystemTreeNodeIT        parent_it                     = m_SystemTreeNodes.find( systemTreeNode );
    SystemTreeNodeProperty& new_system_tree_node_property = *( new SystemTreeNodeProperty );

    new_system_tree_node_property.name  = name;
    new_system_tree_node_property.type  = type;
    new_system_tree_node_property.value = value;

    parent_it->second->systemTreeNodeProperties.push_back( &new_system_tree_node_property );

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefSystemTreeNodeDomain( OTF2_SystemTreeNodeRef systemTreeNode,
                                                OTF2_SystemTreeDomain  systemTreeDomain )
{
    SystemTreeNodeIT      parent_it                   = m_SystemTreeNodes.find( systemTreeNode );
    SystemTreeNodeDomain& new_system_tree_node_domain = *( new SystemTreeNodeDomain );

    new_system_tree_node_domain.systemTreeDomain = systemTreeDomain;

    parent_it->second->systemTreeNodeDomains.push_back( &new_system_tree_node_domain );

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefLocationGroupProperty( OTF2_LocationGroupRef locationGroup,
                                                 OTF2_StringRef        name,
                                                 OTF2_Type             type,
                                                 OTF2_AttributeValue   value )
{
    LocationGroupIT        parent_it                   = m_LocationGroups.find( locationGroup );
    LocationGroupProperty& new_location_group_property = *( new LocationGroupProperty );

    new_location_group_property.name  = name;
    new_location_group_property.type  = type;
    new_location_group_property.value = value;

    parent_it->second->locationGroupProperties.push_back( &new_location_group_property );

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefLocationProperty( OTF2_LocationRef    location,
                                            OTF2_StringRef      name,
                                            OTF2_Type           type,
                                            OTF2_AttributeValue value )
{
    LocationIT        parent_it             = m_Locations.find( location );
    LocationProperty& new_location_property = *( new LocationProperty );

    new_location_property.name  = name;
    new_location_property.type  = type;
    new_location_property.value = value;

    parent_it->second->locationProperties.push_back( &new_location_property );

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefCartCoordinate( OTF2_CartTopologyRef cartTopology,
                                          uint32_t             rank,
                                          uint8_t              numberOfDimensions,
                                          const uint32_t*      coordinates )
{
    CartTopologyIT  parent_it           = m_CartTopologies.find( cartTopology );
    CartCoordinate& new_cart_coordinate = *( new CartCoordinate );

    new_cart_coordinate.rank = rank;
    std::vector< uint32_t > coordinates_vector( coordinates, coordinates + numberOfDimensions );
    new_cart_coordinate.coordinates = coordinates_vector;

    parent_it->second->cartCoordinates.push_back( &new_cart_coordinate );

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefCallingContextProperty( OTF2_CallingContextRef callingContext,
                                                  OTF2_StringRef         name,
                                                  OTF2_Type              type,
                                                  OTF2_AttributeValue    value )
{
    CallingContextIT        parent_it                    = m_CallingContexts.find( callingContext );
    CallingContextProperty& new_calling_context_property = *( new CallingContextProperty );

    new_calling_context_property.name  = name;
    new_calling_context_property.type  = type;
    new_calling_context_property.value = value;

    parent_it->second->callingContextProperties.push_back( &new_calling_context_property );

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefIoFileProperty( OTF2_IoFileRef      ioFile,
                                          OTF2_StringRef      name,
                                          OTF2_Type           type,
                                          OTF2_AttributeValue value )
{
    IoFileIT        parent_it            = m_IoFiles.find( ioFile );
    IoFileProperty& new_io_file_property = *( new IoFileProperty );

    new_io_file_property.name  = name;
    new_io_file_property.type  = type;
    new_io_file_property.value = value;

    parent_it->second->ioFileProperties.push_back( &new_io_file_property );

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefIoPreCreatedHandleState( OTF2_IoHandleRef  ioHandle,
                                                   OTF2_IoAccessMode mode,
                                                   OTF2_IoStatusFlag statusFlags )
{
    IoHandleIT               parent_it                       = m_IoHandles.find( ioHandle );
    IoPreCreatedHandleState& new_io_pre_created_handle_state = *( new IoPreCreatedHandleState );

    new_io_pre_created_handle_state.mode        = mode;
    new_io_pre_created_handle_state.statusFlags = statusFlags;

    parent_it->second->ioPreCreatedHandleStates.push_back( &new_io_pre_created_handle_state );

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefCallpathParameter( OTF2_CallpathRef    callpath,
                                             OTF2_ParameterRef   parameter,
                                             OTF2_Type           type,
                                             OTF2_AttributeValue value )
{
    CallpathIT         parent_it              = m_Callpaths.find( callpath );
    CallpathParameter& new_callpath_parameter = *( new CallpathParameter );

    new_callpath_parameter.parameter = parameter;
    new_callpath_parameter.type      = type;
    new_callpath_parameter.value     = value;

    parent_it->second->callpathParameters.push_back( &new_callpath_parameter );

    return OTF2_CALLBACK_SUCCESS;
}


OTF2_CallbackCode
Otf2Definitions::HandleDefIoParadigm( OTF2_IoParadigmRef             self,
                                      OTF2_StringRef                 identification,
                                      OTF2_StringRef                 name,
                                      OTF2_IoParadigmClass           ioParadigmClass,
                                      OTF2_IoParadigmFlag            ioParadigmFlags,
                                      uint8_t                        numberOfProperties,
                                      const OTF2_IoParadigmProperty* properties,
                                      const OTF2_Type*               types,
                                      const OTF2_AttributeValue*     values )
{
    IoParadigm& new_io_paradigm = *( new IoParadigm );

    new_io_paradigm.identification  = m_Strings[ identification ];
    new_io_paradigm.name            = m_Strings[ name ];
    new_io_paradigm.ioParadigmClass = ioParadigmClass;
    new_io_paradigm.ioParadigmFlags = ioParadigmFlags;
    std::vector< OTF2_IoParadigmProperty > properties_vector( properties, properties + numberOfProperties );
    new_io_paradigm.properties = properties_vector;
    std::vector< OTF2_Type > types_vector( types, types + numberOfProperties );
    new_io_paradigm.types = types_vector;
    std::vector< OTF2_AttributeValue > values_vector( values, values + numberOfProperties );
    new_io_paradigm.values = values_vector;

    m_IoParadigms[ self ] = &new_io_paradigm;

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefAttribute( OTF2_AttributeRef self,
                                     OTF2_StringRef    name,
                                     OTF2_StringRef    description,
                                     OTF2_Type         type )
{
    Attribute& new_attribute = *( new Attribute );

    new_attribute.name        = m_Strings[ name ];
    new_attribute.description = m_Strings[ description ];
    new_attribute.type        = type;

    m_Attributes[ self ] = &new_attribute;

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefSystemTreeNode( OTF2_SystemTreeNodeRef self,
                                          OTF2_StringRef         name,
                                          OTF2_StringRef         className,
                                          OTF2_SystemTreeNodeRef parent )
{
    SystemTreeNode& new_system_tree_node = *( new SystemTreeNode );

    new_system_tree_node.name      = m_Strings[ name ];
    new_system_tree_node.className = m_Strings[ className ];
    new_system_tree_node.parent    = parent;

    m_SystemTreeNodes[ self ] = &new_system_tree_node;

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefLocationGroup( OTF2_LocationGroupRef  self,
                                         OTF2_StringRef         name,
                                         OTF2_LocationGroupType locationGroupType,
                                         OTF2_SystemTreeNodeRef systemTreeParent )
{
    LocationGroup& new_location_group = *( new LocationGroup );

    new_location_group.name              = m_Strings[ name ];
    new_location_group.locationGroupType = locationGroupType;
    new_location_group.systemTreeParent  = systemTreeParent;

    m_LocationGroups[ self ] = &new_location_group;

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefLocation( OTF2_LocationRef      self,
                                    OTF2_StringRef        name,
                                    OTF2_LocationType     locationType,
                                    uint64_t              numberOfEvents,
                                    OTF2_LocationGroupRef locationGroup )
{
    Location& new_location = *( new Location );

    new_location.name           = m_Strings[ name ];
    new_location.locationType   = locationType;
    new_location.numberOfEvents = numberOfEvents;
    new_location.locationGroup  = locationGroup;

    m_Locations[ self ] = &new_location;

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefRegion( OTF2_RegionRef  self,
                                  OTF2_StringRef  name,
                                  OTF2_StringRef  canonicalName,
                                  OTF2_StringRef  description,
                                  OTF2_RegionRole regionRole,
                                  OTF2_Paradigm   paradigm,
                                  OTF2_RegionFlag regionFlags,
                                  OTF2_StringRef  sourceFile,
                                  uint32_t        beginLineNumber,
                                  uint32_t        endLineNumber )
{
    Region& new_region = *( new Region );

    new_region.name            = m_Strings[ name ];
    new_region.canonicalName   = m_Strings[ canonicalName ];
    new_region.description     = m_Strings[ description ];
    new_region.regionRole      = regionRole;
    new_region.paradigm        = paradigm;
    new_region.regionFlags     = regionFlags;
    new_region.sourceFile      = m_Strings[ sourceFile ];
    new_region.beginLineNumber = beginLineNumber;
    new_region.endLineNumber   = endLineNumber;

    m_Regions[ self ] = &new_region;

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefCallsite( OTF2_CallsiteRef self,
                                    OTF2_StringRef   sourceFile,
                                    uint32_t         lineNumber,
                                    OTF2_RegionRef   enteredRegion,
                                    OTF2_RegionRef   leftRegion )
{
    Callsite& new_callsite = *( new Callsite );

    new_callsite.sourceFile    = m_Strings[ sourceFile ];
    new_callsite.lineNumber    = lineNumber;
    new_callsite.enteredRegion = enteredRegion;
    new_callsite.leftRegion    = leftRegion;

    m_Callsites[ self ] = &new_callsite;

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefCallpath( OTF2_CallpathRef self,
                                    OTF2_CallpathRef parent,
                                    OTF2_RegionRef   region )
{
    Callpath& new_callpath = *( new Callpath );

    new_callpath.parent = parent;
    new_callpath.region = region;

    m_Callpaths[ self ] = &new_callpath;

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefGroup( OTF2_GroupRef   self,
                                 OTF2_StringRef  name,
                                 OTF2_GroupType  groupType,
                                 OTF2_Paradigm   paradigm,
                                 OTF2_GroupFlag  groupFlags,
                                 uint32_t        numberOfMembers,
                                 const uint64_t* members )
{
    Group& new_group = *( new Group );

    new_group.name       = m_Strings[ name ];
    new_group.groupType  = groupType;
    new_group.paradigm   = paradigm;
    new_group.groupFlags = groupFlags;
    std::vector< uint64_t > members_vector( members, members + numberOfMembers );
    new_group.members = members_vector;

    m_Groups[ self ] = &new_group;

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefMetricMember( OTF2_MetricMemberRef self,
                                        OTF2_StringRef       name,
                                        OTF2_StringRef       description,
                                        OTF2_MetricType      metricType,
                                        OTF2_MetricMode      metricMode,
                                        OTF2_Type            valueType,
                                        OTF2_Base            base,
                                        int64_t              exponent,
                                        OTF2_StringRef       unit )
{
    MetricMember& new_metric_member = *( new MetricMember );

    new_metric_member.name        = m_Strings[ name ];
    new_metric_member.description = m_Strings[ description ];
    new_metric_member.metricType  = metricType;
    new_metric_member.metricMode  = metricMode;
    new_metric_member.valueType   = valueType;
    new_metric_member.base        = base;
    new_metric_member.exponent    = exponent;
    new_metric_member.unit        = m_Strings[ unit ];

    m_MetricMembers[ self ] = &new_metric_member;

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefMetricClass( OTF2_MetricRef              self,
                                       uint8_t                     numberOfMetrics,
                                       const OTF2_MetricMemberRef* metricMembers,
                                       OTF2_MetricOccurrence       metricOccurrence,
                                       OTF2_RecorderKind           recorderKind )
{
    MetricClass& new_metric_class = *( new MetricClass );

    new_metric_class.metricType = METRIC_CLASS;

    std::vector< OTF2_MetricMemberRef > metricMembers_vector( metricMembers, metricMembers + numberOfMetrics );
    new_metric_class.metricMembers    = metricMembers_vector;
    new_metric_class.metricOccurrence = metricOccurrence;
    new_metric_class.recorderKind     = recorderKind;

    m_Metrics[ self ] = &new_metric_class;

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefMetricInstance( OTF2_MetricRef   self,
                                          OTF2_MetricRef   metricClass,
                                          OTF2_LocationRef recorder,
                                          OTF2_MetricScope metricScope,
                                          uint64_t         scope )
{
    MetricInstance& new_metric_instance = *( new MetricInstance );

    new_metric_instance.metricType = METRIC_INSTANCE;

    new_metric_instance.metricClass = metricClass;
    new_metric_instance.recorder    = recorder;
    new_metric_instance.metricScope = metricScope;
    new_metric_instance.scope       = scope;

    m_Metrics[ self ] = &new_metric_instance;

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefComm( OTF2_CommRef   self,
                                OTF2_StringRef name,
                                OTF2_GroupRef  group,
                                OTF2_CommRef   parent )
{
    Comm& new_comm = *( new Comm );

    new_comm.name   = m_Strings[ name ];
    new_comm.group  = group;
    new_comm.parent = parent;

    m_Comms[ self ] = &new_comm;

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefParameter( OTF2_ParameterRef  self,
                                     OTF2_StringRef     name,
                                     OTF2_ParameterType parameterType )
{
    Parameter& new_parameter = *( new Parameter );

    new_parameter.name          = m_Strings[ name ];
    new_parameter.parameterType = parameterType;

    m_Parameters[ self ] = &new_parameter;

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefRmaWin( OTF2_RmaWinRef self,
                                  OTF2_StringRef name,
                                  OTF2_CommRef   comm )
{
    RmaWin& new_rma_win = *( new RmaWin );

    new_rma_win.name = m_Strings[ name ];
    new_rma_win.comm = comm;

    m_RmaWins[ self ] = &new_rma_win;

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefCartDimension( OTF2_CartDimensionRef self,
                                         OTF2_StringRef        name,
                                         uint32_t              size,
                                         OTF2_CartPeriodicity  cartPeriodicity )
{
    CartDimension& new_cart_dimension = *( new CartDimension );

    new_cart_dimension.name            = m_Strings[ name ];
    new_cart_dimension.size            = size;
    new_cart_dimension.cartPeriodicity = cartPeriodicity;

    m_CartDimensions[ self ] = &new_cart_dimension;

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefCartTopology( OTF2_CartTopologyRef         self,
                                        OTF2_StringRef               name,
                                        OTF2_CommRef                 communicator,
                                        uint8_t                      numberOfDimensions,
                                        const OTF2_CartDimensionRef* cartDimensions )
{
    CartTopology& new_cart_topology = *( new CartTopology );

    new_cart_topology.name         = m_Strings[ name ];
    new_cart_topology.communicator = communicator;
    std::vector< OTF2_CartDimensionRef > cartDimensions_vector( cartDimensions, cartDimensions + numberOfDimensions );
    new_cart_topology.cartDimensions = cartDimensions_vector;

    m_CartTopologies[ self ] = &new_cart_topology;

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefSourceCodeLocation( OTF2_SourceCodeLocationRef self,
                                              OTF2_StringRef             file,
                                              uint32_t                   lineNumber )
{
    SourceCodeLocation& new_source_code_location = *( new SourceCodeLocation );

    new_source_code_location.file       = m_Strings[ file ];
    new_source_code_location.lineNumber = lineNumber;

    m_SourceCodeLocations[ self ] = &new_source_code_location;

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefCallingContext( OTF2_CallingContextRef     self,
                                          OTF2_RegionRef             region,
                                          OTF2_SourceCodeLocationRef sourceCodeLocation,
                                          OTF2_CallingContextRef     parent )
{
    CallingContext& new_calling_context = *( new CallingContext );

    new_calling_context.region             = region;
    new_calling_context.sourceCodeLocation = sourceCodeLocation;
    new_calling_context.parent             = parent;

    m_CallingContexts[ self ] = &new_calling_context;

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefInterruptGenerator( OTF2_InterruptGeneratorRef  self,
                                              OTF2_StringRef              name,
                                              OTF2_InterruptGeneratorMode interruptGeneratorMode,
                                              OTF2_Base                   base,
                                              int64_t                     exponent,
                                              uint64_t                    period )
{
    InterruptGenerator& new_interrupt_generator = *( new InterruptGenerator );

    new_interrupt_generator.name                   = m_Strings[ name ];
    new_interrupt_generator.interruptGeneratorMode = interruptGeneratorMode;
    new_interrupt_generator.base                   = base;
    new_interrupt_generator.exponent               = exponent;
    new_interrupt_generator.period                 = period;

    m_InterruptGenerators[ self ] = &new_interrupt_generator;

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefIoRegularFile( OTF2_IoFileRef         self,
                                         OTF2_StringRef         name,
                                         OTF2_SystemTreeNodeRef scope )
{
    IoRegularFile& new_io_regular_file = *( new IoRegularFile );

    new_io_regular_file.ioFileType = IO_REGULAR_FILE;

    new_io_regular_file.name  = m_Strings[ name ];
    new_io_regular_file.scope = scope;

    m_IoFiles[ self ] = &new_io_regular_file;

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefIoDirectory( OTF2_IoFileRef         self,
                                       OTF2_StringRef         name,
                                       OTF2_SystemTreeNodeRef scope )
{
    IoDirectory& new_io_directory = *( new IoDirectory );

    new_io_directory.ioFileType = IO_DIRECTORY;

    new_io_directory.name  = m_Strings[ name ];
    new_io_directory.scope = scope;

    m_IoFiles[ self ] = &new_io_directory;

    return OTF2_CALLBACK_SUCCESS;
}

OTF2_CallbackCode
Otf2Definitions::HandleDefIoHandle( OTF2_IoHandleRef   self,
                                    OTF2_StringRef     name,
                                    OTF2_IoFileRef     file,
                                    OTF2_IoParadigmRef ioParadigm,
                                    OTF2_IoHandleFlag  ioHandleFlags,
                                    OTF2_CommRef       comm,
                                    OTF2_IoHandleRef   parent )
{
    IoHandle& new_io_handle = *( new IoHandle );

    new_io_handle.name          = m_Strings[ name ];
    new_io_handle.file          = file;
    new_io_handle.ioParadigm    = ioParadigm;
    new_io_handle.ioHandleFlags = ioHandleFlags;
    new_io_handle.comm          = comm;
    new_io_handle.parent        = parent;

    m_IoHandles[ self ] = &new_io_handle;

    return OTF2_CALLBACK_SUCCESS;
}


extern "C" {
static OTF2_CallbackCode
clock_properties_forward_cb( void*    userData,
                             uint64_t timerResolution,
                             uint64_t globalOffset,
                             uint64_t traceLength )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefClockProperties( timerResolution,
                                             globalOffset,
                                             traceLength );
}


static OTF2_CallbackCode
paradigm_forward_cb( void*              userData,
                     OTF2_Paradigm      paradigm,
                     OTF2_StringRef     name,
                     OTF2_ParadigmClass paradigmClass )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefParadigm( paradigm,
                                      name,
                                      paradigmClass );
}


static OTF2_CallbackCode
paradigm_property_forward_cb( void*                 userData,
                              OTF2_Paradigm         paradigm,
                              OTF2_ParadigmProperty property,
                              OTF2_Type             type,
                              OTF2_AttributeValue   value )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefParadigmProperty( paradigm,
                                              property,
                                              type,
                                              value );
}


static OTF2_CallbackCode
io_paradigm_forward_cb( void*                          userData,
                        OTF2_IoParadigmRef             self,
                        OTF2_StringRef                 identification,
                        OTF2_StringRef                 name,
                        OTF2_IoParadigmClass           ioParadigmClass,
                        OTF2_IoParadigmFlag            ioParadigmFlags,
                        uint8_t                        numberOfProperties,
                        const OTF2_IoParadigmProperty* properties,
                        const OTF2_Type*               types,
                        const OTF2_AttributeValue*     values )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefIoParadigm( self,
                                        identification,
                                        name,
                                        ioParadigmClass,
                                        ioParadigmFlags,
                                        numberOfProperties,
                                        properties,
                                        types,
                                        values );
}


static OTF2_CallbackCode
string_forward_cb( void*          userData,
                   OTF2_StringRef self,
                   const char*    string )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefString( self,
                                    string );
}


static OTF2_CallbackCode
attribute_forward_cb( void*             userData,
                      OTF2_AttributeRef self,
                      OTF2_StringRef    name,
                      OTF2_StringRef    description,
                      OTF2_Type         type )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefAttribute( self,
                                       name,
                                       description,
                                       type );
}


static OTF2_CallbackCode
system_tree_node_forward_cb( void*                  userData,
                             OTF2_SystemTreeNodeRef self,
                             OTF2_StringRef         name,
                             OTF2_StringRef         className,
                             OTF2_SystemTreeNodeRef parent )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefSystemTreeNode( self,
                                            name,
                                            className,
                                            parent );
}


static OTF2_CallbackCode
location_group_forward_cb( void*                  userData,
                           OTF2_LocationGroupRef  self,
                           OTF2_StringRef         name,
                           OTF2_LocationGroupType locationGroupType,
                           OTF2_SystemTreeNodeRef systemTreeParent )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefLocationGroup( self,
                                           name,
                                           locationGroupType,
                                           systemTreeParent );
}


static OTF2_CallbackCode
location_forward_cb( void*                 userData,
                     OTF2_LocationRef      self,
                     OTF2_StringRef        name,
                     OTF2_LocationType     locationType,
                     uint64_t              numberOfEvents,
                     OTF2_LocationGroupRef locationGroup )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefLocation( self,
                                      name,
                                      locationType,
                                      numberOfEvents,
                                      locationGroup );
}


static OTF2_CallbackCode
region_forward_cb( void*           userData,
                   OTF2_RegionRef  self,
                   OTF2_StringRef  name,
                   OTF2_StringRef  canonicalName,
                   OTF2_StringRef  description,
                   OTF2_RegionRole regionRole,
                   OTF2_Paradigm   paradigm,
                   OTF2_RegionFlag regionFlags,
                   OTF2_StringRef  sourceFile,
                   uint32_t        beginLineNumber,
                   uint32_t        endLineNumber )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefRegion( self,
                                    name,
                                    canonicalName,
                                    description,
                                    regionRole,
                                    paradigm,
                                    regionFlags,
                                    sourceFile,
                                    beginLineNumber,
                                    endLineNumber );
}


static OTF2_CallbackCode
callsite_forward_cb( void*            userData,
                     OTF2_CallsiteRef self,
                     OTF2_StringRef   sourceFile,
                     uint32_t         lineNumber,
                     OTF2_RegionRef   enteredRegion,
                     OTF2_RegionRef   leftRegion )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefCallsite( self,
                                      sourceFile,
                                      lineNumber,
                                      enteredRegion,
                                      leftRegion );
}


static OTF2_CallbackCode
callpath_forward_cb( void*            userData,
                     OTF2_CallpathRef self,
                     OTF2_CallpathRef parent,
                     OTF2_RegionRef   region )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefCallpath( self,
                                      parent,
                                      region );
}


static OTF2_CallbackCode
group_forward_cb( void*           userData,
                  OTF2_GroupRef   self,
                  OTF2_StringRef  name,
                  OTF2_GroupType  groupType,
                  OTF2_Paradigm   paradigm,
                  OTF2_GroupFlag  groupFlags,
                  uint32_t        numberOfMembers,
                  const uint64_t* members )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefGroup( self,
                                   name,
                                   groupType,
                                   paradigm,
                                   groupFlags,
                                   numberOfMembers,
                                   members );
}


static OTF2_CallbackCode
metric_member_forward_cb( void*                userData,
                          OTF2_MetricMemberRef self,
                          OTF2_StringRef       name,
                          OTF2_StringRef       description,
                          OTF2_MetricType      metricType,
                          OTF2_MetricMode      metricMode,
                          OTF2_Type            valueType,
                          OTF2_Base            base,
                          int64_t              exponent,
                          OTF2_StringRef       unit )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefMetricMember( self,
                                          name,
                                          description,
                                          metricType,
                                          metricMode,
                                          valueType,
                                          base,
                                          exponent,
                                          unit );
}


static OTF2_CallbackCode
metric_class_forward_cb( void*                       userData,
                         OTF2_MetricRef              self,
                         uint8_t                     numberOfMetrics,
                         const OTF2_MetricMemberRef* metricMembers,
                         OTF2_MetricOccurrence       metricOccurrence,
                         OTF2_RecorderKind           recorderKind )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefMetricClass( self,
                                         numberOfMetrics,
                                         metricMembers,
                                         metricOccurrence,
                                         recorderKind );
}


static OTF2_CallbackCode
metric_instance_forward_cb( void*            userData,
                            OTF2_MetricRef   self,
                            OTF2_MetricRef   metricClass,
                            OTF2_LocationRef recorder,
                            OTF2_MetricScope metricScope,
                            uint64_t         scope )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefMetricInstance( self,
                                            metricClass,
                                            recorder,
                                            metricScope,
                                            scope );
}


static OTF2_CallbackCode
comm_forward_cb( void*          userData,
                 OTF2_CommRef   self,
                 OTF2_StringRef name,
                 OTF2_GroupRef  group,
                 OTF2_CommRef   parent )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefComm( self,
                                  name,
                                  group,
                                  parent );
}


static OTF2_CallbackCode
parameter_forward_cb( void*              userData,
                      OTF2_ParameterRef  self,
                      OTF2_StringRef     name,
                      OTF2_ParameterType parameterType )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefParameter( self,
                                       name,
                                       parameterType );
}


static OTF2_CallbackCode
rma_win_forward_cb( void*          userData,
                    OTF2_RmaWinRef self,
                    OTF2_StringRef name,
                    OTF2_CommRef   comm )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefRmaWin( self,
                                    name,
                                    comm );
}


static OTF2_CallbackCode
metric_class_recorder_forward_cb( void*            userData,
                                  OTF2_MetricRef   metric,
                                  OTF2_LocationRef recorder )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefMetricClassRecorder( metric,
                                                 recorder );
}


static OTF2_CallbackCode
system_tree_node_property_forward_cb( void*                  userData,
                                      OTF2_SystemTreeNodeRef systemTreeNode,
                                      OTF2_StringRef         name,
                                      OTF2_Type              type,
                                      OTF2_AttributeValue    value )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefSystemTreeNodeProperty( systemTreeNode,
                                                    name,
                                                    type,
                                                    value );
}


static OTF2_CallbackCode
system_tree_node_domain_forward_cb( void*                  userData,
                                    OTF2_SystemTreeNodeRef systemTreeNode,
                                    OTF2_SystemTreeDomain  systemTreeDomain )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefSystemTreeNodeDomain( systemTreeNode,
                                                  systemTreeDomain );
}


static OTF2_CallbackCode
location_group_property_forward_cb( void*                 userData,
                                    OTF2_LocationGroupRef locationGroup,
                                    OTF2_StringRef        name,
                                    OTF2_Type             type,
                                    OTF2_AttributeValue   value )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefLocationGroupProperty( locationGroup,
                                                   name,
                                                   type,
                                                   value );
}


static OTF2_CallbackCode
location_property_forward_cb( void*               userData,
                              OTF2_LocationRef    location,
                              OTF2_StringRef      name,
                              OTF2_Type           type,
                              OTF2_AttributeValue value )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefLocationProperty( location,
                                              name,
                                              type,
                                              value );
}


static OTF2_CallbackCode
cart_dimension_forward_cb( void*                 userData,
                           OTF2_CartDimensionRef self,
                           OTF2_StringRef        name,
                           uint32_t              size,
                           OTF2_CartPeriodicity  cartPeriodicity )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefCartDimension( self,
                                           name,
                                           size,
                                           cartPeriodicity );
}


static OTF2_CallbackCode
cart_topology_forward_cb( void*                        userData,
                          OTF2_CartTopologyRef         self,
                          OTF2_StringRef               name,
                          OTF2_CommRef                 communicator,
                          uint8_t                      numberOfDimensions,
                          const OTF2_CartDimensionRef* cartDimensions )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefCartTopology( self,
                                          name,
                                          communicator,
                                          numberOfDimensions,
                                          cartDimensions );
}


static OTF2_CallbackCode
cart_coordinate_forward_cb( void*                userData,
                            OTF2_CartTopologyRef cartTopology,
                            uint32_t             rank,
                            uint8_t              numberOfDimensions,
                            const uint32_t*      coordinates )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefCartCoordinate( cartTopology,
                                            rank,
                                            numberOfDimensions,
                                            coordinates );
}


static OTF2_CallbackCode
source_code_location_forward_cb( void*                      userData,
                                 OTF2_SourceCodeLocationRef self,
                                 OTF2_StringRef             file,
                                 uint32_t                   lineNumber )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefSourceCodeLocation( self,
                                                file,
                                                lineNumber );
}


static OTF2_CallbackCode
calling_context_forward_cb( void*                      userData,
                            OTF2_CallingContextRef     self,
                            OTF2_RegionRef             region,
                            OTF2_SourceCodeLocationRef sourceCodeLocation,
                            OTF2_CallingContextRef     parent )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefCallingContext( self,
                                            region,
                                            sourceCodeLocation,
                                            parent );
}


static OTF2_CallbackCode
calling_context_property_forward_cb( void*                  userData,
                                     OTF2_CallingContextRef callingContext,
                                     OTF2_StringRef         name,
                                     OTF2_Type              type,
                                     OTF2_AttributeValue    value )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefCallingContextProperty( callingContext,
                                                    name,
                                                    type,
                                                    value );
}


static OTF2_CallbackCode
interrupt_generator_forward_cb( void*                       userData,
                                OTF2_InterruptGeneratorRef  self,
                                OTF2_StringRef              name,
                                OTF2_InterruptGeneratorMode interruptGeneratorMode,
                                OTF2_Base                   base,
                                int64_t                     exponent,
                                uint64_t                    period )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefInterruptGenerator( self,
                                                name,
                                                interruptGeneratorMode,
                                                base,
                                                exponent,
                                                period );
}


static OTF2_CallbackCode
io_file_property_forward_cb( void*               userData,
                             OTF2_IoFileRef      ioFile,
                             OTF2_StringRef      name,
                             OTF2_Type           type,
                             OTF2_AttributeValue value )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefIoFileProperty( ioFile,
                                            name,
                                            type,
                                            value );
}


static OTF2_CallbackCode
io_regular_file_forward_cb( void*                  userData,
                            OTF2_IoFileRef         self,
                            OTF2_StringRef         name,
                            OTF2_SystemTreeNodeRef scope )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefIoRegularFile( self,
                                           name,
                                           scope );
}


static OTF2_CallbackCode
io_directory_forward_cb( void*                  userData,
                         OTF2_IoFileRef         self,
                         OTF2_StringRef         name,
                         OTF2_SystemTreeNodeRef scope )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefIoDirectory( self,
                                         name,
                                         scope );
}


static OTF2_CallbackCode
io_handle_forward_cb( void*              userData,
                      OTF2_IoHandleRef   self,
                      OTF2_StringRef     name,
                      OTF2_IoFileRef     file,
                      OTF2_IoParadigmRef ioParadigm,
                      OTF2_IoHandleFlag  ioHandleFlags,
                      OTF2_CommRef       comm,
                      OTF2_IoHandleRef   parent )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefIoHandle( self,
                                      name,
                                      file,
                                      ioParadigm,
                                      ioHandleFlags,
                                      comm,
                                      parent );
}


static OTF2_CallbackCode
io_pre_created_handle_state_forward_cb( void*             userData,
                                        OTF2_IoHandleRef  ioHandle,
                                        OTF2_IoAccessMode mode,
                                        OTF2_IoStatusFlag statusFlags )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefIoPreCreatedHandleState( ioHandle,
                                                     mode,
                                                     statusFlags );
}


static OTF2_CallbackCode
callpath_parameter_forward_cb( void*               userData,
                               OTF2_CallpathRef    callpath,
                               OTF2_ParameterRef   parameter,
                               OTF2_Type           type,
                               OTF2_AttributeValue value )
{
    Otf2Definitions* p_this = ( Otf2Definitions* )userData;

    return p_this->HandleDefCallpathParameter( callpath,
                                               parameter,
                                               type,
                                               value );
}
} // extern "C"

Otf2Definitions::Otf2Definitions( OTF2_Reader* reader ) :
    m_Reader( reader )
{
    OTF2_GlobalDefReader* def_reader = OTF2_Reader_GetGlobalDefReader( m_Reader );

    OTF2_GlobalDefReaderCallbacks* def_callbacks = OTF2_GlobalDefReaderCallbacks_New();

    OTF2_GlobalDefReaderCallbacks_SetClockPropertiesCallback( def_callbacks, clock_properties_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetParadigmCallback( def_callbacks, paradigm_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetParadigmPropertyCallback( def_callbacks, paradigm_property_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetIoParadigmCallback( def_callbacks, io_paradigm_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetStringCallback( def_callbacks, string_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetAttributeCallback( def_callbacks, attribute_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetSystemTreeNodeCallback( def_callbacks, system_tree_node_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetLocationGroupCallback( def_callbacks, location_group_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetLocationCallback( def_callbacks, location_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetRegionCallback( def_callbacks, region_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetCallsiteCallback( def_callbacks, callsite_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetCallpathCallback( def_callbacks, callpath_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetGroupCallback( def_callbacks, group_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetMetricMemberCallback( def_callbacks, metric_member_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetMetricClassCallback( def_callbacks, metric_class_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetMetricInstanceCallback( def_callbacks, metric_instance_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetCommCallback( def_callbacks, comm_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetParameterCallback( def_callbacks, parameter_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetRmaWinCallback( def_callbacks, rma_win_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetMetricClassRecorderCallback( def_callbacks, metric_class_recorder_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetSystemTreeNodePropertyCallback( def_callbacks, system_tree_node_property_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetSystemTreeNodeDomainCallback( def_callbacks, system_tree_node_domain_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetLocationGroupPropertyCallback( def_callbacks, location_group_property_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetLocationPropertyCallback( def_callbacks, location_property_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetCartDimensionCallback( def_callbacks, cart_dimension_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetCartTopologyCallback( def_callbacks, cart_topology_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetCartCoordinateCallback( def_callbacks, cart_coordinate_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetSourceCodeLocationCallback( def_callbacks, source_code_location_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetCallingContextCallback( def_callbacks, calling_context_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetCallingContextPropertyCallback( def_callbacks, calling_context_property_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetInterruptGeneratorCallback( def_callbacks, interrupt_generator_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetIoFilePropertyCallback( def_callbacks, io_file_property_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetIoRegularFileCallback( def_callbacks, io_regular_file_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetIoDirectoryCallback( def_callbacks, io_directory_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetIoHandleCallback( def_callbacks, io_handle_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetIoPreCreatedHandleStateCallback( def_callbacks, io_pre_created_handle_state_forward_cb );
    OTF2_GlobalDefReaderCallbacks_SetCallpathParameterCallback( def_callbacks, callpath_parameter_forward_cb );

    OTF2_Reader_RegisterGlobalDefCallbacks( m_Reader, def_reader, def_callbacks, this );

    uint64_t definitions_read = 0;
    OTF2_Reader_ReadAllGlobalDefinitions( m_Reader, def_reader, &definitions_read );

    OTF2_GlobalDefReaderCallbacks_Delete( def_callbacks );

    OTF2_Reader_CloseGlobalDefReader( m_Reader,
                                      def_reader );
}

Otf2Definitions::~Otf2Definitions()
{
    for ( ClockPropertiesIT it = m_ClockProperties.begin();
          it != m_ClockProperties.end();
          it++ )
    {
        delete *it;
    }

    for ( ParadigmIT it = m_Paradigms.begin();
          it != m_Paradigms.end();
          it++ )
    {
        delete *it;
    }

    for ( ParadigmPropertyIT it = m_ParadigmProperties.begin();
          it != m_ParadigmProperties.end();
          it++ )
    {
        delete *it;
    }

    for ( IoParadigmIT it = m_IoParadigms.begin();
          it != m_IoParadigms.end();
          it++ )
    {
        delete it->second;
    }

    for ( AttributeIT it = m_Attributes.begin();
          it != m_Attributes.end();
          it++ )
    {
        delete it->second;
    }

    for ( SystemTreeNodeIT it = m_SystemTreeNodes.begin();
          it != m_SystemTreeNodes.end();
          it++ )
    {
        for ( SystemTreeNodePropertyIT sit = it->second->systemTreeNodeProperties.begin();
              sit != it->second->systemTreeNodeProperties.end();
              sit++ )
        {
            delete *sit;
        }
        for ( SystemTreeNodeDomainIT sit = it->second->systemTreeNodeDomains.begin();
              sit != it->second->systemTreeNodeDomains.end();
              sit++ )
        {
            delete *sit;
        }
        delete it->second;
    }

    for ( LocationGroupIT it = m_LocationGroups.begin();
          it != m_LocationGroups.end();
          it++ )
    {
        for ( LocationGroupPropertyIT sit = it->second->locationGroupProperties.begin();
              sit != it->second->locationGroupProperties.end();
              sit++ )
        {
            delete *sit;
        }
        delete it->second;
    }

    for ( LocationIT it = m_Locations.begin();
          it != m_Locations.end();
          it++ )
    {
        for ( LocationPropertyIT sit = it->second->locationProperties.begin();
              sit != it->second->locationProperties.end();
              sit++ )
        {
            delete *sit;
        }
        delete it->second;
    }

    for ( RegionIT it = m_Regions.begin();
          it != m_Regions.end();
          it++ )
    {
        delete it->second;
    }

    for ( CallsiteIT it = m_Callsites.begin();
          it != m_Callsites.end();
          it++ )
    {
        delete it->second;
    }

    for ( CallpathIT it = m_Callpaths.begin();
          it != m_Callpaths.end();
          it++ )
    {
        for ( CallpathParameterIT sit = it->second->callpathParameters.begin();
              sit != it->second->callpathParameters.end();
              sit++ )
        {
            delete *sit;
        }
        delete it->second;
    }

    for ( GroupIT it = m_Groups.begin();
          it != m_Groups.end();
          it++ )
    {
        delete it->second;
    }

    for ( MetricMemberIT it = m_MetricMembers.begin();
          it != m_MetricMembers.end();
          it++ )
    {
        delete it->second;
    }

    for ( MetricIT it = m_Metrics.begin();
          it != m_Metrics.end();
          it++ )
    {
        for ( MetricClassRecorderIT sit = it->second->metricClassRecorders.begin();
              sit != it->second->metricClassRecorders.end();
              sit++ )
        {
            delete *sit;
        }
        delete it->second;
    }

    for ( CommIT it = m_Comms.begin();
          it != m_Comms.end();
          it++ )
    {
        delete it->second;
    }

    for ( ParameterIT it = m_Parameters.begin();
          it != m_Parameters.end();
          it++ )
    {
        delete it->second;
    }

    for ( RmaWinIT it = m_RmaWins.begin();
          it != m_RmaWins.end();
          it++ )
    {
        delete it->second;
    }

    for ( CartDimensionIT it = m_CartDimensions.begin();
          it != m_CartDimensions.end();
          it++ )
    {
        delete it->second;
    }

    for ( CartTopologyIT it = m_CartTopologies.begin();
          it != m_CartTopologies.end();
          it++ )
    {
        for ( CartCoordinateIT sit = it->second->cartCoordinates.begin();
              sit != it->second->cartCoordinates.end();
              sit++ )
        {
            delete *sit;
        }
        delete it->second;
    }

    for ( SourceCodeLocationIT it = m_SourceCodeLocations.begin();
          it != m_SourceCodeLocations.end();
          it++ )
    {
        delete it->second;
    }

    for ( CallingContextIT it = m_CallingContexts.begin();
          it != m_CallingContexts.end();
          it++ )
    {
        for ( CallingContextPropertyIT sit = it->second->callingContextProperties.begin();
              sit != it->second->callingContextProperties.end();
              sit++ )
        {
            delete *sit;
        }
        delete it->second;
    }

    for ( InterruptGeneratorIT it = m_InterruptGenerators.begin();
          it != m_InterruptGenerators.end();
          it++ )
    {
        delete it->second;
    }

    for ( IoFileIT it = m_IoFiles.begin();
          it != m_IoFiles.end();
          it++ )
    {
        for ( IoFilePropertyIT sit = it->second->ioFileProperties.begin();
              sit != it->second->ioFileProperties.end();
              sit++ )
        {
            delete *sit;
        }
        delete it->second;
    }

    for ( IoHandleIT it = m_IoHandles.begin();
          it != m_IoHandles.end();
          it++ )
    {
        for ( IoPreCreatedHandleStateIT sit = it->second->ioPreCreatedHandleStates.begin();
              sit != it->second->ioPreCreatedHandleStates.end();
              sit++ )
        {
            delete *sit;
        }
        delete it->second;
    }
}
