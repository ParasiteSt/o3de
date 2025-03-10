/*
 * Copyright (c) Contributors to the Open 3D Engine Project.
 * For complete copyright and license terms please see the LICENSE at the root of this distribution.
 *
 * SPDX-License-Identifier: Apache-2.0 OR MIT
 *
 */

#pragma once

#include <AzCore/Serialization/Json/JsonSerializationResult.h>
#include <AzCore/std/optional.h>
#include <AzCore/Asset/AssetCommon.h>
#include <AzToolsFramework/Prefab/Instance/Instance.h>
#include <AzToolsFramework/Prefab/PrefabDomTypes.h>

namespace AzToolsFramework
{
    namespace Prefab
    {
        class Instance;

        namespace PrefabDomUtils
        {
            inline static constexpr const char* InstancesName = "Instances";
            inline static constexpr const char* PatchesName = "Patches";
            inline static constexpr const char* SourceName = "Source";
            inline static constexpr const char* LinkIdName = "LinkId";
            inline static constexpr const char* EntityIdName = "Id";
            inline static constexpr const char* EntitiesName = "Entities";
            inline static constexpr const char* ContainerEntityName = "ContainerEntity";
            inline static constexpr const char* ComponentsName = "Components";
            inline static constexpr const char* EntityOrderName = "Child Entity Order";
            inline static constexpr const char* TypeName = "$type";

            inline static constexpr const char* PathStartingWithEntities = "/Entities/";
            inline static constexpr const char* PathStartingWithInstances = "/Instances/";
            inline static constexpr const char* PathMatchingEntities = "/Entities";
            inline static constexpr const char* PathMatchingInstances = "/Instances";
            inline static constexpr const char* PathMatchingContainerEntity = "/ContainerEntity";
            inline static constexpr const char* PathMatchingLinkId = "/LinkId";
            inline static constexpr const char* PathMatchingSource = "/Source";

            /**
            * Find Prefab value from given parent value and target value's name.
            * @param parentValue A parent value in Prefab DOM.
            * @param valueName A name of child value of parentValue.
            * @return Reference to the child value of parentValue with name valueName if the child value exists.
            */
            PrefabDomValueReference FindPrefabDomValue(PrefabDomValue& parentValue, const char* valueName);
            PrefabDomValueConstReference FindPrefabDomValue(const PrefabDomValue& parentValue, const char* valueName);

            enum class StoreFlags : uint8_t
            {
                //! No flags used.
                None = 0,

                //! By default an instance will be stored with default values. In cases where we want to store less json without defaults
                //! such as saving to disk, this flag will control that behavior.
                StripDefaultValues = 1 << 0,

                //! We do not save linkIds to file. However when loading a level we want to temporarily save
                //! linkIds to instance dom so any nested prefabs will have linkIds correctly set.
                StripLinkIds = 1 << 1
            };
            AZ_DEFINE_ENUM_BITWISE_OPERATORS(StoreFlags);

            //! The metadata about patches indicating information about the modified instance members.
            struct PatchesMetadata
            {
                AZStd::unordered_set<EntityAlias> m_entitiesToReload;
                AZStd::unordered_set<EntityAlias> m_entitiesToRemove;
                AZStd::unordered_set<InstanceAlias> m_instancesToRemove;
                AZStd::unordered_set<InstanceAlias> m_instancesToAdd;
                AZStd::unordered_set<InstanceAlias> m_instancesToReload;
                bool m_shouldReloadContainerEntity = false;
                bool m_clearAndLoadAllEntities = false;
                bool m_clearAndLoadAllInstances = false;
            };

            /**
            * Stores a valid Prefab Instance within a Prefab Dom. Useful for generating Templates.
            * @param instance The instance to store.
            * @param prefabDom The prefabDom that will be used to store the Instance data.
            * @param flags Controls behavior such as whether to store default values.
            * @return bool on whether the operation succeeded.
            */
            bool StoreInstanceInPrefabDom(const Instance& instance, PrefabDom& prefabDom, StoreFlags flags = StoreFlags::None);

            /**
             * Stores a valid Prefab Instance within a Prefab Dom. Useful for generating Templates.
             * @param instance The instance to store.
             * @param prefabDom The prefabDom that will be used to store the Instance data.
             * @param referencedAssets Collect a list of the assets that are referenced during storing.
             * @param flags Controls behavior such as whether to store default values.
             * @return bool on whether the operation succeeded.
             */
            bool StoreInstanceInPrefabDom(
                const Instance& instance,
                PrefabDom& prefabDom,
                AZStd::vector<AZ::Data::Asset<AZ::Data::AssetData>>& referencedAssets,
                StoreFlags flags = StoreFlags::None);

            /**
            * Stores a valid entity in Prefab Dom format.
            * @param entity The entity to store
            * @param owningInstance The instance owning the passed in entity.
            *                       Used for contextualizing the entity's place in a Prefab hierarchy.
            * @param prefabDom The prefabDom that will be used to store the entity data
            * @param flags controls behavior such as whether to store default values
            * @return bool on whether the operation succeeded
            */
            bool StoreEntityInPrefabDomFormat(const AZ::Entity& entity, Instance& owningInstance, PrefabDom& prefabDom,
                StoreFlags flags = StoreFlags::None);

            enum class LoadFlags : uint8_t
            {
                //! No flags used.
                None = 0,

                //! By default entities will get a stable id when they're deserialized. In cases where the new entities need to be kept
                //! unique, e.g. when they are duplicates of live entities, this flag will assign them a random new id.
                AssignRandomEntityId = 1 << 0,

                //! Identifies the entities modified since the last deserialization and only loads them.
                UseSelectiveDeserialization = 1 << 1,

                //! Adds metadata to track and report deprecated components during deserialization
                ReportDeprecatedComponents = 1 << 2
            };
            AZ_DEFINE_ENUM_BITWISE_OPERATORS(LoadFlags);

            /**
            * Loads a valid Prefab Instance from a Prefab Dom. Useful for generating Instances.
            * @param instance The Instance to load.
            * @param prefabDom The prefabDom that will be used to load the Instance data.
            * @param flags Controls behavior such as random entity id assignment.
            * @return bool on whether the operation succeeded.
            */
            bool LoadInstanceFromPrefabDom(
                Instance& instance, const PrefabDom& prefabDom, LoadFlags flags = LoadFlags::None);

            /**
            * Loads a valid Prefab Instance from a Prefab Dom. Useful for generating Instances.
            * @param instance The Instance to load.
            * @param referencedAssets AZ::Assets discovered during json load are added to this list
            * @param prefabDom The prefabDom that will be used to load the Instance data.
            * @param shouldClearContainers Whether to clear containers in Instance while loading.
            * @return bool on whether the operation succeeded.
            */
            bool LoadInstanceFromPrefabDom(
                Instance& instance, const PrefabDom& prefabDom, AZStd::vector<AZ::Data::Asset<AZ::Data::AssetData>>& referencedAssets,
                LoadFlags flags = LoadFlags::None);

            /**
            * Loads a valid Prefab Instance from a Prefab Dom. Useful for generating Instances.
            * @param instance The Instance to load.
            * @param newlyAddedEntities The new instances added during deserializing the instance. These are the entities found
            *       in the prefabDom.
            * @param prefabDom The prefabDom that will be used to load the Instance data.
            * @param shouldClearContainers Whether to clear containers in Instance while loading.
            * @return bool on whether the operation succeeded.
            */
            bool LoadInstanceFromPrefabDom(
                Instance& instance, EntityList& newlyAddedEntities, const PrefabDom& prefabDom,
                LoadFlags flags = LoadFlags::None);

            inline PrefabDomPath GetPrefabDomInstancePath(const char* instanceName)
            {
                return PrefabDomPath()
                    .Append(InstancesName)
                    .Append(instanceName);
            };

            /**
             * Gets a set of all the template source paths in the given dom.
             * @param prefabDom The DOM to get the template source paths from.
             * @param[out] templateSourcePaths The set of template source paths to populate.
             */
            void GetTemplateSourcePaths(const PrefabDomValue& prefabDom, AZStd::unordered_set<AZ::IO::Path>& templateSourcePaths);

            /**
             * Gets the instances DOM value from the given prefab DOM.
             * 
             * @return the instances DOM value or AZStd::nullopt if it instances can't be found.
             */
            PrefabDomValueConstReference GetInstancesValue(const PrefabDomValue& prefabDom);

            AZ::JsonSerializationResult::ResultCode ApplyPatches(
                PrefabDomValue& prefabDomToApplyPatchesOn,
                PrefabDom::AllocatorType& allocator,
                const PrefabDomValue& patches);

             /**
             * Gets the instances DOM value from the given prefab DOM.
             *
             * @return the instances DOM value or AZStd::nullopt if its instances can't be found.
             */
            PrefabDomValueReference GetInstancesValue(PrefabDomValue& prefabDom);

            //! Identifies instance members modified by inspecting the patches provided.
            //! @param patches The patches to inspect.
            //! @return PatchesMetada The metadata object indicating which instance members get modified with the provided patches.
            PatchesMetadata IdentifyModifiedInstanceMembers(const PrefabDom& patches);

            /**
             * Prints the contents of the given prefab DOM value to the debug output console in a readable format.
             * @param printMessage The message that will be printed before printing the PrefabDomValue
             * @param prefabDomValue The DOM value to be printed. A 'PrefabDom' type can also be passed into this variable.
             */
            void PrintPrefabDomValue(
                [[maybe_unused]] const AZStd::string_view printMessage,
                [[maybe_unused]] const AzToolsFramework::Prefab::PrefabDomValue& prefabDomValue);

            AZStd::string PrefabDomValueToString(const PrefabDomValue& prefabDomValue);

            //! Adds a nested instance to the prefab DOM and optionally initialize its contents.
            //! @param prefabDom The prefab DOM to update.
            //! @param nestedInstanceAlias The alias of the nested instance to be added.
            //! @param nestedInstanceDom An optional value to assign to the added nested instance in the prefab DOM.
            void AddNestedInstance(
                PrefabDom& prefabDom,
                const InstanceAlias& nestedInstanceAlias,
                PrefabDomValueConstReference nestedInstanceDom = AZStd::nullopt);

            //! Runs through PrefabDom structure analyzing its general correctness, and checks that
            //! nested entity objects have non-empty parent values in their TransformComponents.
            //! Logs errors found.
            //! In case a nested entity object has the empty parent alias, reassigns the container entity alias as the parent.
            //! This may happen, for example, when loading a spoiled prefab stored with earlier O3DE versions.
            //! @param prefabDom The prefab DOM to check and conditionally update. Changes will be applied in place.
            //! @return True if all checks were successful, otherwise false. 
            bool SubstituteInvalidParentsInEntities(PrefabDom& prefabDom);

            //! An empty struct for passing to JsonSerializerSettings.m_metadata that is consumed by InstanceSerializer::Store.
            //! If present in metadata, linkIds will be stored to instance dom.
            struct LinkIdMetadata
            {
                AZ_RTTI(LinkIdMetadata, "{8FF7D299-14E3-41D4-90C5-393A240FAE7C}");

                virtual ~LinkIdMetadata() {}
            };

            //! An empty struct to pass to the JsonDeserializerSettings, which will be used to identify whether we should selectively
            //! deserialize only modified entities.
            struct InstanceDomMetadata
            {
                AZ_RTTI(InstanceDomMetadata, "{4B509C7B-91B6-4C5E-9696-F7E2C67B6E1B}");
                virtual ~InstanceDomMetadata() {}
            };
        } // namespace PrefabDomUtils
    } // namespace Prefab
} // namespace AzToolsFramework

